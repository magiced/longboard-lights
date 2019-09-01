import csv
import sys
import time
import calendar
import matplotlib.pyplot as plt

rawXadc = []
rawYadc = []
rawZadc = []

mapX = []
mapY = []
mapZ = []

red = []
green = []
blue = []

"""
The float() conversion throws an error when the string input is blank e.g. ''
When there is no data the csv writer in the CAN interface logger and James' 
parsing tool saves the column as '', so to save having to open the file in excel
this handy function sorts it for you!
"""
def parseCsvValIntoFloat(strInput):
    if strInput == '':
        strInput = '0'
    return float(strInput)

def plot3seriesCSV(csv_in):
    # get data from csv file for specific columns

    rawXadc = []
    rawYadc = []
    rawZadc = []

    print("Opening CSV file...")

    csvFile = open(csv_in)
    csvReader = csv.reader(csvFile)
    csvData = list(csvReader)
    csvFile.close()

    print("Extracting pack data from CSV Data...")

    #TODO - 
    # if len(csvData[3]) == 3:
    for i in range(len(csvData)):
        if i == 0:
            continue    #skip header
        rawXadc.append(parseCsvValIntoFloat(csvData[i][0]))
        rawYadc.append(parseCsvValIntoFloat(csvData[i][1]))
        rawZadc.append(parseCsvValIntoFloat(csvData[i][2]))
    # elif len(csvData[3]) == 6:
    #     for i in range(len(csvData)):
    #         if i == 0:
    #             continue    #skip header
    #         mapX.append(parseCsvValIntoFloat(csvData[i][3]))
    #         mapY.append(parseCsvValIntoFloat(csvData[i][4]))
    #         mapZ.append(parseCsvValIntoFloat(csvData[i][5]))
    # elif len(csvData[3]) == 9:
    #     for i in range(len(csvData)):
    #         if i == 0:
    #             continue    #skip header
    #         red.append(parseCsvValIntoFloat(csvData[i][6]))
    #         green.append(parseCsvValIntoFloat(csvData[i][7]))
    #         blue.append(parseCsvValIntoFloat(csvData[i][8]))

    print("Configuring Graph...")

    #https://matplotlib.org/api/_as_gen/matplotlib.pyplot.subplot.html
    fig, topGraph = plt.subplots()

    topGraph.grid(color = 'tab:gray', linestyle='-', linewidth=1)

    topGraph.set(ylabel='ADC val',title = csv_in)

    #colour names come from the X11 colour standard:
    #https://en.wikipedia.org/wiki/X11_color_names
    #https://matplotlib.org/api/_as_gen/matplotlib.pyplot.plot.html
    #topGraph2.plot(Time,balBits,color = 'grey', label = 'Balancing')

    topGraph.plot(rawXadc, label = 'X adc', color='red')
    topGraph.plot(rawYadc, label = 'Y adc', color='limegreen')
    topGraph.plot(rawZadc, label = 'Z adc', color='blue')

    #https://matplotlib.org/tutorials/intermediate/legend_guide.html
    #https://matplotlib.org/api/_as_gen/matplotlib.pyplot.legend.html#matplotlib.pyplot.legend
    topGraph.legend(loc = 'best')
    print('Drawing Graph...')
    plt.show()
    quit()
    destroy()   
    

def plot8seriesCSV(csv_in):
    # get data from csv file for specific columns

    rawXadc = []
    rawYadc = []
    rawZadc = []

    filtXadc = []
    filtYadc = []
    filtZadc = []

    pitch = []
    roll = []
    surge = []


    print("Opening CSV file...")

    csvFile = open(csv_in)
    csvReader = csv.reader(csvFile)
    csvData = list(csvReader)
    csvFile.close()

    print("Extracting pack data from CSV Data...")

    #TODO - 
    # if len(csvData[3]) == 3:
    for i in range(len(csvData)):
        if i == 0:
            continue    #skip header
        rawXadc.append(parseCsvValIntoFloat(csvData[i][0]))
        rawYadc.append(parseCsvValIntoFloat(csvData[i][1]))
        rawZadc.append(parseCsvValIntoFloat(csvData[i][2]))

        filtXadc.append(parseCsvValIntoFloat(csvData[i][3]))
        filtYadc.append(parseCsvValIntoFloat(csvData[i][4]))
        filtZadc.append(parseCsvValIntoFloat(csvData[i][5]))

        pitch.append(parseCsvValIntoFloat(csvData[i][6]))
        roll.append(parseCsvValIntoFloat(csvData[i][7]))
        # surge.append(parseCsvValIntoFloat(csvData[i][8]))

    print("Configuring Graph...")

    #https://matplotlib.org/api/_as_gen/matplotlib.pyplot.subplot.html
    fig, topGraph = plt.subplots()
    topGraph2 = topGraph.twinx()

    topGraph.grid(color = 'tab:gray', linestyle='-', linewidth=1)

    topGraph.set(ylabel='ADC val',title = csv_in)

    #colour names come from the X11 colour standard:
    #https://matplotlib.org/examples/color/named_colors.html
    #https://matplotlib.org/api/_as_gen/matplotlib.pyplot.plot.html
    #topGraph2.plot(Time,balBits,color = 'grey', label = 'Balancing')

    topGraph.plot(rawXadc, label = 'raw X adc', color='coral')
    topGraph.plot(rawYadc, label = 'raw Y adc', color='skyblue')
    topGraph.plot(rawZadc, label = 'raw Z adc', color='palegreen')

    topGraph.plot(filtXadc, label = 'filt X adc', color='red')
    topGraph.plot(filtYadc, label = 'filt Y adc', color='cadetblue')
    topGraph.plot(filtZadc, label = 'filt Z adc', color='limegreen')

    topGraph2.plot(pitch, label = 'pitch', color='deeppink')
    topGraph2.plot(roll, label = 'roll', color='orangered')
    #topGraph2.plot(surge, label = 'surge', color='goldenrod')

    #https://matplotlib.org/tutorials/intermediate/legend_guide.html
    #https://matplotlib.org/api/_as_gen/matplotlib.pyplot.legend.html#matplotlib.pyplot.legend
    topGraph.legend(loc = 'best')
    topGraph2.legend(loc = 'best')
    print('Drawing Graph...')
    plt.show()
    quit()
    destroy()   
    
def plotAllColsCSV(csv_in):
    # get data from csv file for specific columns

    data = []

    print("Opening CSV file...")

    csvFile = open(csv_in)
    csvReader = csv.reader(csvFile)
    csvData = list(csvReader)
    csvFile.close()

    print("Extracting pack data from CSV Data...")

    #TODO - 
    # if len(csvData[3]) == 3:
    for i in range(len(csvData)):
        if i == 0:
            continue    #skip header
        rawXadc.append(parseCsvValIntoFloat(csvData[i][0]))


    print("Configuring Graph...")

    #https://matplotlib.org/api/_as_gen/matplotlib.pyplot.subplot.html
    fig, topGraph = plt.subplots()
    topGraph2 = topGraph.twinx()

    topGraph.grid(color = 'tab:gray', linestyle='-', linewidth=1)

    topGraph.set(ylabel='ADC val',title = csv_in)

    #colour names come from the X11 colour standard:
    #https://en.wikipedia.org/wiki/X11_color_names
    #https://matplotlib.org/api/_as_gen/matplotlib.pyplot.plot.html
    #topGraph2.plot(Time,balBits,color = 'grey', label = 'Balancing')

    topGraph.plot(rawXadc, label = 'raw X adc', color='coral', zorder = 0)
    topGraph.plot(rawYadc, label = 'raw Y adc', color='light blue', zorder = 5)
    topGraph.plot(rawZadc, label = 'raw Z adc', color='pale green', zorder = 10)

    topGraph.plot(filtXadc, label = 'filt X adc', color='red', zorder = 15)
    topGraph.plot(filtYadc, label = 'filt Y adc', color='cadet blue', zorder = 20)
    topGraph.plot(filtZadc, label = 'filt Z adc', color='lime green', zorder = 25)

    topGraph2.plot(pitch, label = 'pitch', color='deep pink', zorder = 30)
    topGraph2.plot(roll, label = 'roll', color='', zorder = 35)
    #topGraph2.plot(surge, label = 'surge', color='goldenrod', zorder = 0)

    #https://matplotlib.org/tutorials/intermediate/legend_guide.html
    #https://matplotlib.org/api/_as_gen/matplotlib.pyplot.legend.html#matplotlib.pyplot.legend
    topGraph.legend(loc = 'best')
    print('Drawing Graph...')
    plt.show()
    quit()
    destroy()   

if __name__ == '__main__':
    plot3seriesCSV(str(sys.argv[1]))






