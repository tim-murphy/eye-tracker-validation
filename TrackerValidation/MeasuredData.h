// Abstract factory class for storage of measured data.
// Tim Murphy <tim@murphy.org> 2021

#ifndef MEASUREDDATA_H
#define MEASUREDDATA_H

#include <chrono>
#include <string>

class MeasuredData
{
  private:
    // A human-readable name for the tracker being used. This is intentionally
    // unrestricted to allow for custom labels for the same tracker if, for
    // example, testing is done with different settings.
    std::string name;

  public:
    MeasuredData(std::string trackerName);
    virtual ~MeasuredData();

    // Write the data to the buffer or datastore (whatever that may be)
    virtual bool writeData(
        std::chrono::time_point<std::chrono::system_clock> timestamp,
        unsigned int targetNumber,
        unsigned int xTarget,
        unsigned int yTarget,
        unsigned int xActual,
        unsigned int yActual) = 0;

    // If using a buffer, write the buffered data to the datastore. If not
    // overloaded, this method is a no-op.
    virtual void writeBuffer();

    // create a data store of the given type.
    // @throws std::runtime_error if type does not match a known type.
    // @param path the filesystem path to the store, or "" if not applicable
    static MeasuredData *create(const std::string &type,
                                const std::string &trackerName,
                                std::string path = "");

    // -- getters -- //
    const std::string &getName() const;
};

#endif // defined MEASUREDDATA_H