import tkinter as tk
import tkinter.ttk as ttk
import serial
import serial.tools.list_ports
import time
import csv
from tkinter import scrolledtext
from tkinter import filedialog
from os import path
import csv3axisplotter as csv3
# import plotCSV

b_logging = False
last_b_logging = False
outfile = 1
csvWriter = None

def open_close_port():
    ser.open()
    # # try:
    # write_to_textbox(ser.name)
    # write_to_textbox(cbo_ports.get())

    # write_to_textbox('Opened serial port {0} at {1}'.format(cbo_ports.get(),cbo_speed.get()))
    # # except:
    # #     write_to_textbox('ERROR: Failed to open serial port {0} at {1}'.format(cbo_ports.get(),cbo_speed.get()))

def write_to_textbox(instring):
    textbox.configure(state='normal')

    textbox.insert('end',instring + '\n')
    textbox.configure(state='disabled')
    textbox.see(tk.END)

def clear_textbox():
    textbox.delete(1.0,'end')

def send_serial():
    data = tx_text.get()
    try:
        ser.write(data.encode('utf-8'))
        textbox.insert('end','SEND: ' + data + '\n')
    except NameError:
        write_to_textbox('ERROR: No Serial Port Open!')

def fill_combo_boxes():
    pass
    #cbo_ports['vals.list_ports.comports()']

def toggle_log():
    global b_logging
    global outfile
    global csvWriter
    if b_logging:

        b_logging = False
        btn_log['text'] = 'Start Log'
        btn_log['bg'] = 'green'
    else:
        """ open file dialog
            create/save file
            change text on logging button
            change b_logging to True
            """
        outfile = tk.filedialog.asksaveasfile(initialdir=path.dirname(__file__), mode="w", initialfile=create_datetime_filename(), filetypes=(("CSV Files","*.csv"),("Text Files","*.txt")))

        csvWriter = csv.writer(outfile)
        # if type(outfile) == None:
        #     b_logging = False
        #     btn_log['text'] = 'Start Log'
        #     print('outfile none')
        # else:
        #     b_logging = True
        #     print('startfile trlas
        b_logging = True
        btn_log['text'] = 'Stop Log'
        btn_log['bg'] = 'red'

def check_and_print_serial():
    global b_logging
    global last_b_logging
    global outfile
    if ser.is_open:
        while ser.in_waiting > 0:
            data = ser.readline().decode('ascii').strip().split()
            if b_timestamp.get():
                out_text = str(time.time()) + ' ' + ' '.join([str(i) for i in data])
            else:
                #' '.join([str(v) for v in L])
                out_text = ' '.join([str(i) for i in data])
            textbox.insert('end',out_text + '\n')
            lbl_serial_rx['text'] = str(data)
            if b_logging:
                # csvWriter.writerow(out_text)
                csvWriter.writerow(data)
                # outfile.write(out_text + '\n')

    if b_logging == False and last_b_logging != b_logging:
        outfile.close()

    last_b_logging = b_logging            
    window.after(10,check_and_print_serial)

def create_datetime_filename():
    return time.strftime("%Y-%m-%d_%H-%M-%S", time.localtime())

def plot_last_log():
    try:
        csv3.plot3seriesCSV(outfile.name)
        # csv3.plot8seriesCSV(outfile.name)
    except AttributeError:
        print("ERROR - You haven't taken a log yet!")

def plot_chosen_log():
    infilename = tk.filedialog.askopenfilename(initialdir=path.dirname(__file__), filetypes=(("CSV Files","*.csv"),("Text Files","*.txt"),("All Files","*")))
    print(infilename)
    try:
        csv3.plot3seriesCSV(infilename)
        # csv3.plot8seriesCSV(infilename)
    except AttributeError:
        print("ERROR - You haven't taken a log yet!")


""" DEFINE GUI WIDGETS """

window = tk.Tk()
window.title('Serial Logger')

cbo_ports = ttk.Combobox(window)
cbo_ports['value'] = ('/dev/ttyUSB0','/dev/ttyUSB1','COM17')
cbo_ports.current(0)

cbo_speed = ttk.Combobox(window)
cbo_speed['values'] = (300,1200,2400,4800,9600,19200,38400,57600,74880,115200,230400)
cbo_speed.current(4)

btn_open = tk.Button(window, text='OPEN',command=open_close_port)

textbox = tk.scrolledtext.ScrolledText(window, width=50, height=20)
btn_clear = tk.Button(window, text='CLEAR', command=clear_textbox)
b_timestamp = tk.BooleanVar()
b_timestamp.set(False)
cb_timestamp = tk.Checkbutton(window, text='Add timestamp', var=b_timestamp)

tx_text = tk.Entry(window, width=30)
tx_text.focus()
btn_transmit = tk.Button(window, text='TRANSMIT', command=send_serial)

btn_log = tk.Button(window, text='Start Log', command=toggle_log)
btn_graph = tk.Button(window,text='Plot last log', command=plot_last_log)
btn_plot_choose_log = tk.Button(window, text="Choose a log to plot", command=plot_chosen_log)

lbl_serial_rx = tk.Label(window, text='serial comes out here')

""" DEFINE GUI LAYOUT """
cbo_ports.grid(column=0, row=0)
cbo_speed.grid(column=1, row=0)
btn_open.grid(column=2, row=0)

textbox.grid(column=0, row=1, columnspan=3)

btn_clear.grid(column=0, row=2)
cb_timestamp.grid(column=1, row=2)

tx_text.grid(column=0, row=3, columnspan=2)
btn_transmit.grid(column=2, row=3, sticky='W')

btn_log.grid(column=0, row=4)
btn_graph.grid(column=2, row=4)

lbl_serial_rx.grid_columnconfigure(0,weight=1)
lbl_serial_rx.grid(column=0, row=5, sticky='e')
btn_plot_choose_log.grid(row=5, column=2)

ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=4)
# ser.close()
# ser.port(cbo_ports.get())
# ser.baudrate(cbo_speed.get())
# ser.open()

fill_combo_boxes()
check_and_print_serial()
window.mainloop()
