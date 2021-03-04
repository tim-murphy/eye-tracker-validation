// Validation processing.
// Tim Murphy <tim@murphy.org> 2021

#include "Validator.h"

#include "ScreenPositionStore.h"
#include "MeasuredData.h"
#include "ValidatorUIOpenGL.h"

#include <chrono>
#include <cstdlib> // for rand()
#include <iostream>
#include <cmath>
#include <sstream>
#include <thread>

Validator::Validator(const ValidatorConfig &conf)
    : config(conf), ui(nullptr), data(nullptr),
      showingTarget(false), targetIndex(0),
      trackerDataCollector(nullptr)
{
    cursorPosition = new ScreenPositionStore();
    gazePosition = new ScreenPositionStore();
    targetPosition = new ScreenPositionStore();
    dimensions = std::make_pair(config.cols, config.rows);

    // initialise the test counts
    testCount.resize(
        (static_cast<size_t>(config.cols) * static_cast<size_t>(config.rows)), 0);

    if (config.outputFile == "")
    {
        data = MeasuredData::create("cout", config.trackerLabel);
    }
    else
    {
        data = MeasuredData::create("file", config.trackerLabel,
                                    config.outputFile);
    }

    valPtr = this;
}

Validator::~Validator()
{
    valPtr = nullptr;
    delete data;                    data = nullptr;
    delete gazePosition;            gazePosition = nullptr;
    delete cursorPosition;          cursorPosition = nullptr;
    delete targetPosition;          targetPosition = nullptr;
    delete ui;                      ui = nullptr;
    delete trackerDataCollector;    trackerDataCollector = nullptr;
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

// mouse click event processing for the UI
void onClickFunc(int button, int state, int x, int y)
{
    if (Validator::valPtr == nullptr)
    {
        throw std::runtime_error("onClickFunc called when thread not running!");
    }

    if (ValidatorUIOpenGL::mouseClickEvent(button, state))
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

    ui = new ValidatorUIOpenGL(getTargetSize(), getTargetType(),
                               argcp, argvp);
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
    cursorPosition->setCurrentPosition(pos);
}

void Validator::setGazePos(std::pair<unsigned int, unsigned int> pos)
{
    gazePosition->setCurrentPosition(pos);
}

bool Validator::recordMeasurement()
{
    // if the cursor didn't click the target, ignore it.
    if (!cursorOverTarget())
    {
        return false;
    }

    std::chrono::time_point<std::chrono::system_clock> currTime
        = std::chrono::system_clock::now();

    // lock both objects together - we want the data to be as close
    // as we can
    gazePosition->lock();
    cursorPosition->lock();
    std::pair<unsigned int, unsigned int>tPos = getTargetPos();
    std::pair<unsigned int, unsigned int>cPos = getGazePos();
    gazePosition->unlock();
    cursorPosition->unlock();

    if (data->writeData(currTime, getTargetIndex(),
                        tPos.first, tPos.second,
                        cPos.first, cPos.second))
    {
        ++testCount[getTargetIndex()];
        return true;
    }

    // if we get here, the measurement wasn't recorded.
    return false;
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
    unsigned int row = index / (getDimensions().first);
    unsigned int col = index % (getDimensions().first);

    return std::make_pair(col, row);
}

void Validator::showTarget()
{
    // sanity check - don't try to show a new target if we've already finished.
    if (testingDone())
    {
        throw std::runtime_error(
            "Cannot show new target - testing already complete.");
    }

    // Pick a random position which has not been fully tested yet.
    // Note that we're not seeding the random number generator as it's not
    // really necessary - we don't care if each participant gets the same
    // 'random' sequence - and it removes the need for ctime or another
    // seeding dependency.
    unsigned int randomIndex;
    do
    {
        randomIndex = rand() % testCount.size();
    } while (testCount[randomIndex] >= getReps());

    setTargetPos(randomIndex);
    ui->showTarget(getTargetPos());
    setShowingTarget(true);
}

void Validator::startTrackerDataCollector(const std::string &command,
                                          const TrackerConfig &conf)
{
    if (trackerDataCollector != nullptr)
    {
        delete trackerDataCollector;
    }
    trackerDataCollector = TrackerDataCollector::create(command,
                                                        *gazePosition,
                                                        conf);
    trackerDataCollector->run();
}

void Validator::stopTrackerDataCollector()
{
    if (trackerDataCollector == nullptr)
    {
        return;
    }

    trackerDataCollector->stop();
    delete trackerDataCollector;
    trackerDataCollector = nullptr;
}

std::pair<unsigned int, unsigned int> Validator::getCursorPos() const
{
    return cursorPosition->getCurrentPosition();
}

std::pair<unsigned int, unsigned int> Validator::getTargetPos() const
{
    return targetPosition->getCurrentPosition();
}

std::pair<unsigned int, unsigned int> Validator::getGazePos() const
{
    return gazePosition->getCurrentPosition();
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
    std::pair<unsigned int, unsigned int> screenRes = ui->getScreenRes();
    unsigned int cellWidth = screenRes.first / getDimensions().first;
    unsigned int cellHeight = screenRes.second / getDimensions().second;

    unsigned int xTarget = (cellWidth * colRowPair.first) + (cellWidth / 2);
    unsigned int yTarget = (cellHeight * colRowPair.second) + (cellHeight / 2);

    // the target will be in the middle of this cell
    targetPosition->setCurrentPosition(std::make_pair(xTarget, yTarget));
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
