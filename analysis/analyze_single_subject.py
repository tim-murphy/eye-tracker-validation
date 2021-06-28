# Take a CSV file for a single subject and do the following:
#  - graph the points, with different sets for each label/tracker pair
#  - calculate the accuracy
#  - calculate the precision

import csv
import matplotlib.cm as pltcm
import matplotlib.colors as pltcolors
import matplotlib.pyplot as plt
import os
import sys

DATA_COLS = {
    "Label": 0,
    "Subject": 1,
    "Tracker": 2,
    "Timestamp": 3,
    "Target-ID": 4,
    "Target-X": 5,
    "Target-Y": 6,
    "Cursor-X": 7,
    "Cursor-Y": 8,
    "Actual-X": 9,
    "Actual-Y": 10
}

PLOT_SIZE = (1920, 1080)
PLOT_DPI = 100
COLOURMAP = "nipy_spectral"

def printUsage():
    print("Usage: " + sys.argv[0] + " <data_csv> [<graph_output_png>]")

if __name__ == '__main__':
    print("===================================")
    print("  Data analysis for a single user")
    print("===================================")
    print("")

    # parse command line arguments
    if len(sys.argv) < 2:
        printUsage()
        sys.exit(1)

    data_csv = sys.argv[1]
    graph_output_png = None

    if len(sys.argv) > 2:
        graph_output_png = sys.argv[2]

        if os.path.exists(graph_output_png):
            response = None
            while response not in ("y", "n"):
                response = input("WARNING: The graph output path \"" +\
                                 graph_output_png + "\" already exists. Would "\
                                 "you like to overwrite it? [y/n]: ").lower()

            if response == "n":
                print("Exiting")
                sys.exit(1)
            else:
                print("File will be overwritten")

    # error checking
    if not os.path.exists(data_csv):
        print("ERROR: data file does not exist: " + data_csv)
        printUsage()
        sys.exit(1)


    #################################
    ## Data extraction starts here ##
    #################################

    print("Using input file: " + data_csv)

    if graph_output_png is None:
        print("Displaying output on screen")
    else:
        print("Writing output to: " + graph_output_png)

    # load the data
    # NOTE: we're not using a dictreader here as we can't guarantee there will
    #       be a header row present.
    raw_data = None
    with open(data_csv, 'r') as csvfile:
        raw_data = list(csv.reader(csvfile, delimiter=','))

    targets = {}
    subject = None
    invalid_rows = set()
    subject_data = {}
    for index, row in enumerate(raw_data):
        # remove the header rows
        # note: we assume the row is a header if the last element is not a number
        try:
            _ = int(row[-1])
        except:
            print("Ignoring header row: " + str(row))
            invalid_rows.add(index)
            continue

        # make sure we're only processing one subject
        if subject is None:
            subject = row[DATA_COLS["Subject"]]
            print("Subject: " + subject)
        else:
            if row[DATA_COLS["Subject"]] != subject:
                print("ERROR: multiple subjects found in dataset: "\
                      + subject + " and " + row[DATA_COLS["Subject"]])
                sys.exit(1)

        # extract the targets
        target_id = int(row[DATA_COLS["Target-ID"]])
        target_coords = (int(row[DATA_COLS["Target-X"]]),\
                        int(row[DATA_COLS["Target-Y"]]))

        if target_id in targets:
            # make sure the data is consistent
            if targets[target_id] != target_coords:
                print("ERROR: inconsintent data for target " + str(target_id)\
                      + ": coords recorded at " + str(targets[target_id])\
                      + " and " + str(target_coords))
                sys.exit(1)
        else:
            targets[target_id] = target_coords

        # extract subject data
        identifier = (row[DATA_COLS["Tracker"]], row[DATA_COLS["Label"]])
        coords = (int(row[DATA_COLS["Target-ID"]]),\
                  int(row[DATA_COLS["Actual-X"]]),\
                  int(row[DATA_COLS["Actual-Y"]]))

        if coords[1] < 0 or coords[1] > PLOT_SIZE[0] or\
           coords[2] < 0 or coords[2] > PLOT_SIZE[1]:
            print("Ignoring invalid data: " + str(coords))
            invalid_rows.add(index)
            continue

        if identifier in subject_data:
            subject_data[identifier] += [coords]
        else:
            subject_data[identifier] = [coords]

    # FIXME do we need to do this?
    # remove the header rows
    raw_data = [row for index, row in enumerate(raw_data) if index not in invalid_rows]

    print(str(len(raw_data)) + " data rows found")
    print(str(len(targets)) + " target locations found")

    print("")
    print("Targets:")
    print(targets)

    print("")
    print("Subject data:")
    print(subject_data)

    ##########################
    ## Plotting starts here ##
    ##########################

    plt.figure(figsize=(PLOT_SIZE[0]/PLOT_DPI, PLOT_SIZE[1]/PLOT_DPI), dpi=PLOT_DPI)
    plt.axis([0, 1920, 0 ,1080])
    # plt.axis('off')
    plt.gca().invert_yaxis()
    plt.subplots_adjust(left=0.05, right=0.70, top=0.825, bottom=0.175)

    # first plot the target positions
    for targ in targets:
        coords = targets[targ]
        plt.scatter(coords[0], coords[1], marker='x', color=(0.0, 0.0, 0.0, 0.5), s=750)

    # now plot each series
    colours = pltcm.ScalarMappable(norm=pltcolors.Normalize(vmin=0, vmax=len(subject_data) - 1),\
                                   cmap=plt.get_cmap(COLOURMAP))
    for i, subj in enumerate(subject_data):
        colour = (colours.to_rgba(i))
        print("Plotting " + str(subj) + " with colour " + str(colour))
        label = subj[0] + " :: " + subj[1]
        x_coords = []
        y_coords = []
        for coords in subject_data[subj]:
            x_coords.append(coords[1])
            y_coords.append(coords[2])
        plt.scatter(x_coords, y_coords, marker='o', color=colour, s=5, label=label)

    plt.suptitle("Single User Data", fontsize=40)
    plt.title("Subject: " + subject, fontsize=30)
    plt.legend(loc="upper left", bbox_to_anchor=(1.05, 1.01), title="Legend", fontsize="medium", title_fontsize="large")

    if graph_output_png is None:
        plt.show()
    else:
        print("Writing plot to file")
        plt.savefig(graph_output_png, dpi=PLOT_DPI)

    print("Finished. Have a nice day :)")

# EOF
