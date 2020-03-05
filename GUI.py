import tkinter
from tkinter import *
from tkinter.filedialog import askopenfilename
import json
import itertools


def load_object_window():
    filename = askopenfilename()
    with open(filename) as file:
        ans = json.load(file)
    return ans


class PopupWindow(object):
    def __init__(self, master, title=''):
        top = self.top = Toplevel(master)
        self.l = Label(top, text=title)
        self.l.grid(row=0, columnspan=2)
        self.e = Entry(top)
        self.e.grid(row=1, columnspan=2)
        self.b_commit = Button(top, text='Save', command=self.commit)
        self.b_cancel = Button(top, text='Cancel', command=self.cleanup)
        self.b_commit.grid(row=2, column=0)
        self.b_cancel.grid(row=2, column=1)

    def commit(self):
        self.value = self.e.get()
        self.top.destroy()

    def cleanup(self):
        self.value = None
        self.top.destroy()


class TableData:

    def __init__(self, rows, cols, col_dicts=None):
        self._rows = rows
        self._cols = cols
        self._col_dicts = col_dicts if col_dicts is not None else [None for _ in cols]


class TableMap(TableData):

    def __init__(self, map, key_dict=None, val_dict=None, key_name='key', val_name='value'):
        rows = [(k, map[k]) for k in map.keys()]
        cols = [key_name, val_name]
        col_dicts = [key_dict, val_dict]
        TableData.__init__(self, rows, cols, col_dicts)


class Table(Frame):

    @staticmethod
    def remap_list(lst, reverse_dict):
        return [reverse_dict[elem] for elem in lst]

    def FrameWidth(self, event):
        canvas_width = event.width
        self.canvas.itemconfig(self.canvas_frame, width=canvas_width)

    def OnFrameConfigure(self, event):
        self.canvas.config(scrollregion=self.canvas.bbox("all"))

    def update_command_wrapper(self):
        for row_ind, row in self.get_rows_without_nones(include_indices=True):
            mutable_row = list(row)
            for col_ind, col in enumerate(row):
                if col_ind in self.entries:
                    col_type = type(row[col_ind])
                    try:
                        val = col_type(self.entries[col_ind][row_ind].get())
                    except Exception:
                        tkinter.messagebox.showerror("Table update",
                                                     "Invalid value entered, only {} is accepted.".format(col_type))
                        return
                    mutable_row[col_ind] = col_type(val)
            self._rows[row_ind] = tuple(mutable_row)
        #self._update_cmd(self._rows)

    def get_rows_without_nones(self, include_indices=False):
        iterable = filter(
            lambda a: a is not None, self._rows) if not include_indices else filter(
            lambda a: a[1] is not None, enumerate(self._rows))
        for elem in iterable:
            yield elem

    def delete_row(self):
        if self.selected is None:
            return
        row = self.selected
        widget_list = self.row_to_widgets_map[row]
        for widget in widget_list:
            widget.destroy()
        self._rows[row] = None
        self.selected = None

    def save_command(self):
        top = self
        w = PopupWindow(top, 'Choose file name')
        top.wait_window(w.top)
        if w.value is not None:
            f_name = w.value
            rows = [row for row in self._rows if row is not None]
            with open(f_name, 'w') as jsonOutput:
                json.dump(rows, jsonOutput)

    def load_from_file(self, file_name):
        with open(file_name) as json_file:
            new_rows = json.load(json_file)
            for row in new_rows:
                if len(row) != self.cols_num:
                    raise BaseException("Invalid json file")
            self.fill_table(self._rows)

    def load_command(self):
        filename = askopenfilename()
        try:
            self.empty_table()
            self.load_from_file(filename)
        except Exception:
            print(sys.exc_info())
            tkinter.messagebox.showerror("Load from file", "Invalid file")

    def select_row(self, e):
        defaultbg = self.cget('bg')
        row = self.widget_to_rows_map[e.widget]
        widget_list = self.row_to_widgets_map[row]
        if row == self.selected:
            for widget in widget_list:
                widget.configure(background=defaultbg)
            self.selected = None
            return
        for widget in widget_list:
            widget.configure(background="lightblue")
        if self.selected is not None:
            former_selected_row = self.row_to_widgets_map[self.selected]
            for widget in former_selected_row:
                widget.configure(background=defaultbg)
        self.selected = row

    def __init__(self, rows, cols, mutable_cols, *args, **kwargs):
        self._update_cmd = None
        self._print_funcs = None
        self._print_cols = None
        if 'update_cmd' in kwargs:
            self._update_cmd = kwargs['update_cmd']
            del kwargs['update_cmd']  # Must delete since superclass constructor would throw an exception otherwise
        if 'print_funcs' in kwargs:
            self._print_funcs = kwargs['print_funcs']
            self._print_cols = kwargs['print_cols']
            del kwargs['print_funcs']
            del kwargs['print_cols']
        Frame.__init__(self, *args, **kwargs)
        self.is_dict = len(rows) > 0 and isinstance(rows[0], dict)
        if self.is_dict:
            cols = rows[0].keys()
            mutable_cols = [False]*len(cols)
        self.rows_num = len(rows)
        self.cols_num = len(cols)
        self.cols = cols
        self.selected = None
        self.grid_columnconfigure(0, weight=1)
        self.grid_rowconfigure(0, weight=1)
        self._rows = rows
        self.deleted_rows = []
        self._mutable_cols = mutable_cols
        self.buttonFrame = None
        self.canvas = None
        self.sensorsStatsFrame = None
        self.canvas_frame = None
        self.widget_to_rows_map = {}
        self.row_to_widgets_map = {}
        self.entries = {}
        self.create_scroll_bars()
        self.create_header(cols)
        self.fill_table(rows)
        self.create_button_frame()

    def create_scroll_bars(self):
        sensors_frame = Frame(self)
        sensors_frame.grid(row=0, rowspan=5, sticky="nsew")
        sensors_frame.grid_rowconfigure(0, weight=1)
        sensors_frame.grid_columnconfigure(0, weight=1)
        self.canvas = Canvas(sensors_frame)
        self.sensorsStatsFrame = Frame(self.canvas)
        self.canvas.grid_columnconfigure(0, weight=1)
        self.sensorsStatsFrame.grid_columnconfigure(0, weight=1)
        verticalscrollbar = Scrollbar(sensors_frame, orient=VERTICAL, command=self.canvas.yview)
        self.canvas.configure(yscrollcommand=verticalscrollbar.set)
        self.canvas.grid(column=0, sticky="nsew")
        verticalscrollbar.grid(row=0, column=1, sticky="nsew")
        horizontalscrollbar = Scrollbar(sensors_frame, orient=HORIZONTAL, command=self.canvas.xview)
        self.canvas.configure(xscrollcommand=horizontalscrollbar.set)
        horizontalscrollbar.grid(row=1, column=0, sticky="nsew")
        self.canvas_frame = self.canvas.create_window((0, 0), window=self.sensorsStatsFrame, anchor='nw')
        self.sensorsStatsFrame.bind("<Configure>", self.OnFrameConfigure)

    def create_header(self, cols):
        for ind, col in enumerate(cols):
            header_label = Label(self.sensorsStatsFrame, text=col,
                                 font='Helvetica 18 bold')
            header_label.grid(row=0, column=ind)

    def print_func(self, col):
        if self._print_funcs is None or col not in self._print_cols:
            return lambda s: s
        ind = self._print_cols.index(col)
        return self._print_funcs[ind]

    def empty_table(self):
        for row in self.row_to_widgets_map.keys():
            row_widgets = self.row_to_widgets_map[row]
            for widget in row_widgets:
                widget.destroy()
        self.row_to_widgets_map = {}
        self.widget_to_rows_map = {}
        self.rows_num = 0
        self.selected = None
        self.entries = {}

    def fill_table(self, rows):
        self._rows = rows
        self.rows_num = len(rows)
        for col in range(self.cols_num):
            if self._mutable_cols[col]:
                self.entries[col] = []
        for row, (col, col_name) in itertools.product(range(self.rows_num), enumerate(self.cols)):
            print_func = self.print_func(col)
            key = col if not self.is_dict else col_name
            if row not in self.row_to_widgets_map:
                self.row_to_widgets_map[row] = []
            if len(rows[row]) != self.cols_num:
                raise BaseException("Columns number in row {} don't match".format(row))
            if self._mutable_cols[col]:
                cell_label = Entry(self.sensorsStatsFrame)
                cell_label.insert(END, print_func(rows[row][key]))
                self.entries[col].append(cell_label)
            else:
                cell_label = Label(self.sensorsStatsFrame, text=print_func(rows[row][key]))
            cell_label.grid(row=row+1, column=col)
            cell_label.bind("<Button-1>", lambda e: self.select_row(e))
            self.widget_to_rows_map[cell_label] = row
            self.row_to_widgets_map[row].append(cell_label)

    def create_button_frame(self):
        self.buttonFrame = Frame(self)
        self.buttonFrame.grid(row=6)
        delete_button = Button(self.buttonFrame, text='Delete', command=lambda: self.delete_row())
        delete_button.grid(row=0, column=1)
        save_button = Button(self.buttonFrame, text='Save', command=lambda: self.save_command())
        save_button.grid(row=0, column=2)
        load_button = Button(self.buttonFrame, text='Load', command=lambda: self.load_command())
        load_button.grid(row=0, column=3)
        if any(self._mutable_cols):
            change_button = Button(self.buttonFrame, text='Update', command=lambda: self.update_command_wrapper())
            change_button.grid(row=0, column=0)


class MapTable(Table):

    def __init__(self, map, reverse_dict, *args, **kwargs):
        cols = ['key', 'value']
        if 'cols' in kwargs:
            cols = kwargs['cols']
            del kwargs['cols']
        self.map = self.keys = self.reverse_dict = self.repr_to_key_dict = self.values = self.actual_rows = \
            self.keys_to_repr = None
        self.map = map
        rows = self.init_maps(map, reverse_dict)
        Table.__init__(self, rows, cols, [False, True], *args, **kwargs)

    def init_maps(self, map, reverse_dict):
        self.keys = map.keys()
        self.reverse_dict = reverse_dict
        self.repr_to_key_dict = {tuple(self.remap_list(k, reverse_dict)): k for k in self.keys}
        self.values = [map[k] for k in self.keys]
        self.actual_rows = [(k, v) for k, v in zip(self.keys, self.values)]
        self.keys_to_repr = {tuple(self.remap_list(k, reverse_dict)): k for k in self.keys}
        rows = [(self.remap_list(k, reverse_dict), v) for k, v in self.actual_rows]
        return rows

    def remove_keys_from_map(self):
        deleted_keys = []
        for key in self.keys:
            remapped_key = self.remap_list(key, self.reverse_dict)
            if remapped_key not in [row[0] for row in self.get_rows_without_nones()]:
                deleted_keys.append(key)
        for key in deleted_keys:
            del self.map[key]

    def load_from_file(self, file_name):
        with open(file_name) as json_file:
            new_map = json.load(json_file)
            new_map = {tuple(k): v for k, v in new_map}
            for k in new_map.keys():
                self.map[k] = new_map[k]
            rows = self.init_maps(new_map, self.reverse_dict)
            self.fill_table(rows)

    def save_command(self):
        top = self
        w = PopupWindow(top, 'Choose file name')
        top.wait_window(w.top)
        if w.value is not None:
            f_name = w.value
            self.update_command_wrapper()
            output = [(k, self.map[k]) for k in self.map.keys()]
            with open('{}.json'.format(f_name), 'w') as jsonOutput:
                json.dump(output, jsonOutput)

    def update_keys_in_map(self):
        for row in self.get_rows_without_nones():
            key = self.repr_to_key_dict[tuple(row[0])]
            self.map[key] = row[1]

    def update_command_wrapper(self):
        Table.update_command_wrapper(self)
        self.remove_keys_from_map()
        self.update_keys_in_map()


class HidableFrame(Frame):

    def __init__(self, text, *args, **kwargs):
        Frame.__init__(self, *args, **kwargs)
        self.text = text
        self.visible = IntVar()
        self.vars = {}
        self.frame = None
        self.create_frame()

    def add_var(self, name, desc=None):
        if name in self.vars:
            raise BaseException("Already has this variable")
        if desc is None:
            desc = name
        label = Label(self.frame, text=desc)
        entry = Entry(self.frame)
        label.grid(row=len(self.vars), column=0)
        entry.grid(row=len(self.vars), column=1)
        self.vars[name] = 0

    def create_frame(self):
        self.frame = Frame(self)
        self.frame.grid(row=2, column=0)
        self.frame.grid_remove()

        def toggle_dup_frame():
            if self.visible.get() == 1:
                self.frame.grid()
            else:
                self.frame.grid_remove()

        dup_check = Checkbutton(self, text=self.text, variable=self.visible,
                                command=toggle_dup_frame)
        dup_check.grid(row=1, column=0)


class Checklist(Frame):

    def FrameWidth(self, event):
        canvas_width = event.width
        self.canvas.itemconfig(self.canvas_frame, width=canvas_width)

    def OnFrameConfigure(self, event):
        self.canvas.config(scrollregion=self.canvas.bbox("all"))

    def on_click(self, e):
        row = self.widget_to_row[e.widget]
        self.command(row)

    def __init__(self, elements, *args, **kwargs):
        self.command = lambda i: print(i)
        if 'command' in kwargs:
            self.command = kwargs['command']
            del kwargs['command']
        Frame.__init__(self, *args, **kwargs)
        self.elements = elements
        self.elem_bools = [IntVar() for _ in elements]
        self.grid_columnconfigure(0, weight=1)
        self.grid_rowconfigure(0, weight=1)
        self.checkbuttons = []
        self.selected_all = IntVar()
        self.widget_to_row = {}
        sensorsFrame = Frame(self)
        sensorsFrame.grid(row=0, sticky="nsew")
        sensorsFrame.grid_rowconfigure(0, weight=1)
        sensorsFrame.grid_columnconfigure(0, weight=1)

        self.canvas = Canvas(sensorsFrame)
        self.sensorsStatsFrame = Frame(self.canvas)
        self.canvas.grid_columnconfigure(0, weight=1)
        #self.sensorsStatsFrame.grid_columnconfigure(0, weight=1)

        myscrollbar = Scrollbar(sensorsFrame, orient=VERTICAL, command=self.canvas.yview)
        self.canvas.configure(yscrollcommand=myscrollbar.set)
        self.canvas.grid(column=0, sticky="nsew")
        myscrollbar.grid(row=0, column=1, sticky="nsew")
        for ind, elem in enumerate(elements):
            b = Checkbutton(self.sensorsStatsFrame,
                            var=self.elem_bools[ind])
            lbl = Label(self.sensorsStatsFrame, text=str(elem))
            lbl.bind("<Button-1>", lambda e: self.on_click(e))
            self.widget_to_row[lbl] = ind
            b.grid(column=0, row=ind+1, padx=(100, 0))
            lbl.grid(column=1, row=ind+1)
            self.checkbuttons.append(b)

        all_label = Label(self.sensorsStatsFrame, text="Select all")
        all = Checkbutton(self.sensorsStatsFrame, command=self.select_all,
                          var=self.selected_all)
        all.grid(column=0, row=0)
        all_label.grid(column=1, row=0)

        self.canvas_frame = self.canvas.create_window((0, 0), window=self.sensorsStatsFrame, anchor='nw')
        self.sensorsStatsFrame.bind("<Configure>", self.OnFrameConfigure)
        self.canvas.bind('<Configure>', self.FrameWidth)

    def select_all(self):
        if self.selected_all.get() == 1:
            for b in self.checkbuttons:
                b.select()
        else:
            for b in self.checkbuttons:
                b.deselect()

    def get_selected(self):
        ans = [ind for ind, elem in enumerate(self.elem_bools) if elem.get() == 1]
        return ans
