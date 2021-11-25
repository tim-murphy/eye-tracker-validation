// Validation processing.
// Tim Murphy <tim@murphy.org> 2021

#include "Validator.h"

#include "common.h"
#include "ScreenPositionStore.h"
#include "MeasuredData.h"
#include "ValidatorUIOpenGL.h"

#include <chrono>
#include <cstdlib> // for rand()
#include <ctime> // for time(), used to seed rand()
#include <iostream>
#include <cmath>
#include <sstream>
#include <thread>

Validator::Validator(const ValidatorConfig &conf)
    : config(conf), ui(nullptr), data(nullptr),
      showingTarget(false), targetIndex(0),
      trackerDataCollector(nullptr),
      gazePosThread(nullptr),
      showGaze(true)
{
    cursorPosition = new ScreenPositionStore();
    gazePosition = new ScreenPositionStore();
    targetPosition = new ScreenPositionStore();
    dimensions = std::make_pair(config.cols, config.rows);

    trackerDataCollector = TrackerDataCollector::create(config.tracker,
                                                        *gazePosition,
                                                        config.trackerConfig);

    // initialise the test counts
    // if we're using "corners" target location, increase each dimension by 1
    size_t cols = static_cast<size_t>(config.cols);
    size_t rows = static_cast<size_t>(config.rows);

    if (config.targLocation == "corners")
    {
        ++cols;
        ++rows;
    }

    testCount.resize(cols * rows, 0);

    if (config.outputFile == "")
    {
        data = MeasuredData::create("cout",
                                    config.trackerLabel,
                                    trackerDataCollector->getName(),
                                    config.subject);
    }
    else
    {
        data = MeasuredData::create("file",
                                    config.trackerLabel,
                                    trackerDataCollector->getName(),
                                    config.subject,
                                    config.outputFile);
    }

    valPtr = this;

    gazePosThread = new std::thread(&Validator::collectGazePos, this);

    // seed our random number generator, used to determine the order targets
    // are displayed
    srand(static_cast<unsigned int>(time(NULL)));
}

Validator::~Validator()
{
    if (gazePosThread != nullptr)
    {
        showGaze = false;
        gazePosThread->join();
        delete gazePosThread;       gazePosThread = nullptr;
    }

    valPtr = nullptr;
    delete data;                    data = nullptr;
    delete gazePosition;            gazePosition = nullptr;
    delete cursorPosition;          cursorPosition = nullptr;
    delete targetPosition;          targetPosition = nullptr;
    delete ui;                      ui = nullptr;
    delete trackerDataCollector;    trackerDataCollector = nullptr;
}

void Validator::collectGazePos()
{
    while (showGaze)
    {
        if (ui != nullptr && gazePosition != nullptr)
        {
            std::pair<std::pair<unsigned int, unsigned int>, std::pair<unsigned int, unsigned int> >pos
                = gazePosition->getCurrentPositionRightLeft();
            ui->setGazePos(pos.first, pos.second);
            refreshUI();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

// idle function for the UI - processing done when UI drawing not
// being processed.
Validator *Validator::valPtr = nullptr;
void idleFunc(void)
{
    if (Validator::valPtr != nullptr)
    {
        Validator::valPtr->run();
    }
    else
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void Validator::refreshUI()
{
    if (ui != nullptr)
    {
        ui->refresh();
    }
}

// mouse click event processing for the UI
void onClickFunc(int button, int state, int x, int y)
{
    if (Validator::valPtr == nullptr)
    {
        throw std::runtime_error("onClickFunc called when thread not running!");
    }

    // this should never happen
    if (ValidatorUIOpenGL::getInstance() == nullptr)
    {
        throw std::runtime_error("onClickFunc called before UI was started!");
    }

    if (ValidatorUIOpenGL::getInstance()->mouseClickEvent(button, state))
    {
        Validator::valPtr->setCursorPos(std::make_pair(x, y));

        if (Validator::valPtr->recordMeasurement())
        {
            Validator::valPtr->setShowingTarget(false);
        }
    }
}

void Validator::startUI(int *argcp, char **argvp)
{
    // just in case - for normal use this should not be initialised
    delete ui;

    ui = ValidatorUIOpenGL::create(getTargetSize(), getTargetType(),
                                   argcp, argvp, config.preview);
    ui->setIdleFunc(&idleFunc);
    ui->setMouseFunc(&onClickFunc);
    ui->run();
}

void Validator::stopUI()
{
    // prevent segfault if someone calls this before startUI()
    if (ui != nullptr)
    {
        ui->stop();
    }
}

void Validator::setCursorPos(std::pair<unsigned int, unsigned int> pos)
{
    cursorPosition->setCurrentPositionSingle(pos);
}

void Validator::setGazePos(std::pair<unsigned int, unsigned int> right,
                           std::pair<unsigned int, unsigned int> left)
{
    gazePosition->setCurrentPositionRightLeft(right, left);
}

bool Validator::recordMeasurement()
{
    bool success = false;

    // lock both objects together - we want the data to be as close as we can
    gazePosition->lock();
    cursorPosition->lock();

    // if the cursor didn't click the target, ignore it.
    if (cursorOverTarget())
    {
        std::chrono::time_point<std::chrono::system_clock> currTime
            = std::chrono::system_clock::now();

        std::pair<unsigned int, unsigned int> tPos = getTargetPos();
        std::pair<unsigned int, unsigned int> cPos = getCursorPos();
        std::pair<std::pair<unsigned int, unsigned int>, std::pair<unsigned int, unsigned int> > gPos = getGazePosRightLeft();

        if (data->writeData(currTime, getTargetIndex(),
                            tPos.first, tPos.second,
                            cPos.first, cPos.second,
                            gPos.first.first, gPos.first.second,
                            gPos.second.first, gPos.second.second))
        {
            ++testCount[getTargetIndex()];
            success = true;
        }
    }

    gazePosition->unlock();
    cursorPosition->unlock();

    return success;
}

void Validator::run()
{
    // make sure the ui is initialised
    while (this->ui == nullptr)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    // if we're done testing, clean everything up
    if (testingDone())
    {
        stopUI();
        std::cout << "Validator finished!" << std::endl;
        data->writeBuffer();
        return;
    }

    // if we've got a target on show and are waiting for input, then there's
    // nothing else to be done here
    if (getShowingTarget())
    {
        return;
    }

    // show the next target, if we're ready for it
    if (ui->inTestRoutine())
    {
        showTarget();
    }

    // if we get here then there's nothing to do!
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

bool Validator::testingDone() const
{
    bool done = true;

    // simply loop through the testCount countainer and see if every element
    // is equal to (or greater than - shouldn't happen!) the number of reps.
    for (auto count : testCount)
    {
        if (count < getReps())
        {
            done = false;
            break;
        }
    }

    return done;
}

std::pair<unsigned int, unsigned int>
    Validator::indexToColRow(unsigned int index) const
{
    // these are the configured dimensions
    std::pair<unsigned int, unsigned int> dims = getDimensions();

    // if we're using "corners" target location, increase each dimension by 1
    if (config.targLocation == "corners")
    {
        ++dims.first;
        // we don't need to `++dims.second` as it's never used
    }

    unsigned int row = index / (dims.first);
    unsigned int col = index % (dims.first);

    return std::make_pair(col, row);
}

void Validator::showTarget()
{
    // in preview mode, we show all targets
    if (config.preview)
    {
        for (unsigned int x = 0; x < testCount.size(); ++x)
        {
            setTargetPos(x);
            ui->showTarget(getTargetPos(), x+1 == testCount.size(), x == 0);
        }
    }
    else
    {
        // sanity check - don't try to show a new target if we've finished.
        if (testingDone())
        {
            throw std::runtime_error(
                "Cannot show new target - testing already complete.");
        }

        // Pick a random position which has not been fully tested yet.
        // This was seeded in the constructor.
        unsigned int randomIndex;
        do
        {
            randomIndex = rand() % testCount.size();
        } while (testCount[randomIndex] >= getReps());

        setTargetPos(randomIndex);
        ui->showTarget(getTargetPos());
    }
    setShowingTarget(true);
}

void Validator::startTrackerDataCollector()
{
    trackerDataCollector->run();
}

void Validator::stopTrackerDataCollector()
{
    trackerDataCollector->stop();
}

std::pair<unsigned int, unsigned int> Validator::getCursorPos() const
{
    return cursorPosition->getCurrentPositionSingle();
}

std::pair<unsigned int, unsigned int> Validator::getTargetPos() const
{
    return targetPosition->getCurrentPositionSingle();
}

std::pair<unsigned int, unsigned int> Validator::getGazePosSingle() const
{
    return gazePosition->getCurrentPositionSingle();
}

std::pair<std::pair<unsigned int, unsigned int>, std::pair<unsigned int, unsigned int> > Validator::getGazePosRightLeft() const
{
    return gazePosition->getCurrentPositionRightLeft();
}

bool Validator::cursorOverTarget() const
{
    // we consider the cursor to be over the target if it is within targSize/2
    // pixels (targSize is the diameter of the target). As the target is a
    // circle, we need to do the appropriate math.
    std::pair<unsigned int, unsigned int>targetPos = getTargetPos();
    std::pair<unsigned int, unsigned int>cursorPos = getCursorPos();
    double radius = getTargetSize() / 2.0; // for ease of calculation

    // typecast to ensure the calculation is accurate
    double xTarg = static_cast<double>(targetPos.first);
    double yTarg = static_cast<double>(targetPos.second);
    double xCur = static_cast<double>(cursorPos.first);
    double yCur = static_cast<double>(cursorPos.second);

    // if the cursor is within the bounds of the target (including on the edge)
    // then the following equation will return true:
    //   (x_cur - x_targ)^2  + (y_cur - y_targ)^2 <= radius^2
    return (std::pow(xCur - xTarg, 2.0) + std::pow((yCur - yTarg), 2.0))
                <= std::pow(radius, 2.0);
}

void Validator::setTargetPos(unsigned int index)
{
    // first we need to calculate the row and column.
    std::pair<unsigned int, unsigned int> colRowPair = indexToColRow(index);

    // from this, define the bounding box based on the screen res.
    std::pair<unsigned int, unsigned int> screenRes = common::getScreenRes();

    // subtract the padding (x2 as padded on both sides)
    screenRes.first -= (config.padding * 2);
    screenRes.second -= (config.padding * 2);

    unsigned int cellWidth = screenRes.first / getDimensions().first;
    unsigned int cellHeight = screenRes.second / getDimensions().second;

    unsigned int xTarget = (cellWidth * colRowPair.first) + (cellWidth / 2);
    unsigned int yTarget = (cellHeight * colRowPair.second) + (cellHeight / 2);

    // adjust for padding
    xTarget += config.padding;
    yTarget += config.padding;

    // adjust if using corners cell location
    if (config.targLocation == "corners")
    {
        xTarget -= (cellWidth / 2);
        yTarget -= (cellHeight / 2);
    }

    // the target will be in the middle of this cell
    targetPosition->setCurrentPositionSingle(std::make_pair(xTarget, yTarget));
    targetIndex = index;
}

// -- vanilla getters and setters -- //
unsigned int Validator::getReps() const
{
    return config.repeats;
}

unsigned int Validator::getTargetSize() const
{
    return config.targetSize;
}

const std::string &Validator::getTargetType() const
{
    return config.targType;
}

const std::pair<unsigned int, unsigned int> &Validator::getDimensions() const
{
    return dimensions;
}

const std::string &Validator::getTrackerLabel() const
{
    return config.trackerLabel;
}

bool Validator::getShowingTarget() const
{
    return showingTarget;
}

void Validator::setShowingTarget(bool isShowing)
{
    showingTarget = isShowing;
}

unsigned int Validator::getTargetIndex() const
{
    return targetIndex;
}
