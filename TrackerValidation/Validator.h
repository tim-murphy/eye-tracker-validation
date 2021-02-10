// Validation processing.
// Tim Murphy <tim@murphy.org> 2021

#ifndef VALIDATOR_H
#define VALIDATOR_H

#include "MeasuredData.h"
#include "ScreenPositionStore.h"
#include "TrackerConfig.h"
#include "TrackerDataCollector.h"
#include "ValidatorUI.h"

#include <utility> // for std::pair
#include <string>
#include <vector>

class Validator
{
  private:
    // Are we showing a target (and waiting for user input?)
    bool showingTarget;

    // Data store for measured data.
    MeasuredData *data;

    TrackerDataCollector *trackerDataCollector;

    // The label given to the tracker we will be validating
    std::string trackerLabel;

    // Grid dimensions (cols, rows).
    std::pair<unsigned int, unsigned int> dimensions;

    // How many times are we testing each point?
    unsigned int reps;

    // How big is the target? This is a diameter in pixels.
    unsigned int targSize;

    // Container with a counter for each grid point tested.
    // Storing as a 1-D container for simplicity and speed.
    // Index = (row * (numCols - 1) + col)
    std::vector<unsigned int> testCount;

    // Current position data for gaze.
    ScreenPositionStore *gazePosition;

    // Current position data for the cursor.
    ScreenPositionStore *cursorPosition;

    // Current target position.
    ScreenPositionStore *targetPosition;
    unsigned int targetIndex;

    // User interface
    ValidatorUI* ui;

    // Get the current cursor position.
    std::pair<unsigned int, unsigned int> getCursorPos() const;

    // Get the current target position.
    std::pair<unsigned int, unsigned int> getTargetPos() const;

    // Get the current gaze position.
    std::pair<unsigned int, unsigned int> getGazePos() const;

    // Is the cursor of the current target?
    bool cursorOverTarget() const;

    // Have all points been tested yet?
    bool testingDone() const;

    // Show the next target. The position of the next target is randomised
    // based on cells left to test.
    void showTarget();

    // convert a position index into a (col, row) pair (zero indexed).
    std::pair<unsigned int, unsigned int> indexToColRow(unsigned int index) const;

  public:

    // Workaround to get the UI idle functions working
    static Validator* valPtr;

    // Constructor
    // The screen will be split into a grid of dimensions (columns, rows).
    // A target will be placed in the center of each grid cell, and cells will
    // be tested repeats times. The target will be a circle of targetSize
    // pixels in diameter.
    // @param columns number of columns to split the screen into
    // @param rows the number of rows to split the screen into
    // @param repeats the number of times each cell is tested
    // @param targetSize the diameter of the circular target, in pixels
    // @param trackerLabel a descriptive label written with the data
    Validator(unsigned int columns,
              unsigned int rows,
              unsigned int repeats,
              unsigned int targetSize,
              std::string trackerLabel);

    // Destructor
    ~Validator();

    // Run the validation routine.
    void run();

    // start/stop the UI
    void startUI(int* argcp, char** argvp);
    void stopUI();

    // start/stop the gaze tracker data collector
    // @param tracker the tracker being used in this experiment
    // @throws std::runtime_error if the tracker couldn't start
    void startTrackerDataCollector(const std::string &tracker,
                                   const TrackerConfig &config);
    void stopTrackerDataCollector();

    // a public wrapper to the cursor position setter as this may
    // be set by an external object
    void setCursorPos(std::pair<unsigned int, unsigned int> pos);

    // a public wrapper to the gaze position setter as this may be
    // set by an external object
    void setGazePos(std::pair<unsigned int, unsigned int> pos);

    // Record a measurement if the cursor is over the target.
    // @returns true if measurement was recorded.
    // @returns false if the cursor was not over the target.
    bool recordMeasurement();

    // -- getters and setters -- //
    unsigned int getReps() const;
    unsigned int getTargetSize() const;
    const std::pair<unsigned int, unsigned int> &getDimensions() const;
    unsigned int getTargetIndex() const;
    const std::string &getTrackerLabel() const;
    bool getShowingTarget() const;
    void setTargetPos(unsigned int index);
    void setShowingTarget(bool isShowing);
};

#endif // defined VALIDATOR_H
