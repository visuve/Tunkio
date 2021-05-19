import ctypes
import datetime
import enum
import math
import os
import signal
import sys
import threading
import time

KUURA_STRING = ctypes.c_char_p if sys.platform != "win32" else ctypes.c_wchar_p


def human_readable(num, suffix='B'):
    magnitude = int(math.floor(math.log(num, 1024)))
    val = num / math.pow(1024, magnitude)
    if magnitude > 7:
        return '{:.1f} {}{}'.format(val, 'Yi', suffix)
    return '{:3.2f} {}{}'.format(val, ['', 'Ki', 'Mi', 'Gi', 'Ti', 'Pi', 'Ei', 'Zi'][magnitude], suffix)


def update_progress():
    while (Kuura.keep_running):
        time.sleep(0.1)

        start_time = Kuura.start_time
        bytes_written = Kuura.bytes_written
        bytes_beginning = Kuura.bytes_beginning

        if start_time <= 0.0 or bytes_beginning <= 0.0 or bytes_written <= 0.0:
            continue

        seconds_taken = time.perf_counter() - start_time
        bytes_per_second = float(bytes_written) / seconds_taken
        bytes_left = bytes_beginning - bytes_written
        seconds_left = bytes_left / bytes_per_second

        sys.stdout.write("{} written.".format(human_readable(bytes_written)))
        sys.stdout.write(" {} left.".format(human_readable(bytes_left)))
        sys.stdout.write(" {:.2%} Complete.".format(bytes_written / bytes_beginning))
        sys.stdout.write(" Taken: {}.".format(datetime.timedelta(seconds=int(seconds_taken))))
        sys.stdout.write(" ETA: {}.".format(datetime.timedelta(seconds=int(seconds_left))))
        sys.stdout.write(" Speed: {}.".format(human_readable(bytes_per_second, 'B/s')))
        sys.stdout.write('\n')

class Kuura:
    keep_running = False
    start_time = 0.0
    bytes_beginning = 0.0
    bytes_written = 0.0
    _update_thread = threading.Thread(target=update_progress)

    class Handle(ctypes.Structure):
        _fields_ = [
            ("Unused", ctypes.c_int32),
        ]

    class TargetType(enum.IntEnum):
        FileOverwrite = ord('f')
        DirectoryOverwrite = ord('d')
        DriveOverwrite = ord('D')

    class FillType(enum.IntEnum):
        ZeroFill = ord('0')
        OneFill = ord('1')
        ByteFill = ord('b')
        SequenceFill = ord('s')
        FileFill = ord('f')
        RandomFill = ord('r')

    def __init__(self, kuura_api_path):
        self.kuura_handle = None
        self.kuura_api = ctypes.CDLL(kuura_api_path)

        kuura_init_fn = self.kuura_api.KuuraInitialize
        kuura_init_fn.restype = ctypes.POINTER(Kuura.Handle)
        kuura_init_fn.argtypes = [ctypes.c_void_p]

        self.kuura_add_target_fn = self.kuura_api.KuuraAddTarget
        self.kuura_add_target_fn.restype = ctypes.c_bool
        self.kuura_add_target_fn.argtypes = [ctypes.POINTER(Kuura.Handle), ctypes.c_int32, KUURA_STRING, ctypes.c_bool]

        self.kuura_add_pass_fn = self.kuura_api.KuuraAddPass
        self.kuura_add_pass_fn.restype = ctypes.c_bool
        self.kuura_add_pass_fn.argtypes = [ctypes.POINTER(Kuura.Handle), ctypes.c_int32, ctypes.c_bool, ctypes.c_char_p]

        self.kuura_run_fn = self.kuura_api.KuuraRun
        self.kuura_run_fn.restype = ctypes.c_bool
        self.kuura_run_fn.argtypes = [ctypes.POINTER(Kuura.Handle)]

        self.kuura_free_fn = self.kuura_api.KuuraFree
        self.kuura_free_fn.argtypes = [ctypes.POINTER(Kuura.Handle)]

        self.kuura_version_fn = self.kuura_api.KuuraVersion
        self.kuura_version_fn.restype = ctypes.c_char_p

        this = ctypes.cast(ctypes.pointer(ctypes.py_object(self)), ctypes.c_void_p)
        self.kuura_handle = kuura_init_fn(this)

        self._init_callbacks(self.kuura_api)

    def _init_callbacks(self, kuura_api):


        kuura_set_overwrite_stared_fn = kuura_api.KuuraSetOverwriteStartedCallback
        kuura_set_overwrite_stared_fn(self.kuura_handle, Kuura._on_overwrite_started)

        kuura_set_target_started_fn = kuura_api.KuuraSetTargetStartedCallback
        kuura_set_target_started_fn(self.kuura_handle, Kuura._on_target_started)

        kuura_set_pass_started_fn = kuura_api.KuuraSetPassStartedCallback
        kuura_set_pass_started_fn(self.kuura_handle, Kuura._on_pass_started)

        kuura_set_progress_fn = kuura_api.KuuraSetProgressCallback
        kuura_set_progress_fn(self.kuura_handle, Kuura._on_progress)

        kuura_set_pass_completed_fn = kuura_api.KuuraSetPassCompletedCallback
        kuura_set_pass_completed_fn(self.kuura_handle, Kuura._on_pass_completed)

        kuura_set_target_completed_fn = kuura_api.KuuraSetTargetCompletedCallback
        kuura_set_target_completed_fn(self.kuura_handle, Kuura._on_target_completed)

        kuura_set_overwrite_completed_fn = kuura_api.KuuraSetOverwriteCompletedCallback
        kuura_set_overwrite_completed_fn(self.kuura_handle, Kuura._on_overwrite_completed)

        kuura_set_error_cb = kuura_api.KuuraSetErrorCallback
        kuura_set_error_cb(self.kuura_handle, Kuura._on_error)


    @staticmethod
    def signal_handler(signalled_number, frame):
        print("Canceled")
        Kuura.keep_running = False

    @staticmethod
    @ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.c_uint16, ctypes.c_uint64)
    def _on_overwrite_started(context, passes, bytes_left):
        print("Overwrite started")
        Kuura.start_time = time.perf_counter()
        Kuura.bytes_beginning = float(bytes_left)
        Kuura._update_thread.start()

    @staticmethod
    @ctypes.CFUNCTYPE(None, ctypes.c_void_p, KUURA_STRING, ctypes.c_uint64)
    def _on_target_started(context, path, bytes_left):
        print("Target started")

    @staticmethod
    @ctypes.CFUNCTYPE(None, ctypes.c_void_p, KUURA_STRING, ctypes.c_uint16)
    def _on_pass_started(context, path, pass_num):
        print("Pass started")

    @staticmethod
    @ctypes.CFUNCTYPE(ctypes.c_bool, ctypes.c_void_p, KUURA_STRING, ctypes.c_uint16, ctypes.c_uint64)
    def _on_progress(context, path, pass_num, bytes_written):
        Kuura.bytes_written = float(bytes_written)
        return Kuura.keep_running

    @staticmethod
    @ctypes.CFUNCTYPE(None, ctypes.c_void_p, KUURA_STRING, ctypes.c_char, ctypes.c_uint16, ctypes.c_uint64, ctypes.c_uint32)
    def _on_error(context, path, phase, pass_num, bytes_written, error_code):
        print(f"ERROR: {os.strerror(error_code)}!")
        Kuura.keep_running = False

    @staticmethod
    @ctypes.CFUNCTYPE(None, ctypes.c_void_p, KUURA_STRING, ctypes.c_uint16)
    def _on_pass_completed(context, path, pass_num):
        print("Pass completed")

    @staticmethod
    @ctypes.CFUNCTYPE(None, ctypes.c_void_p, KUURA_STRING, ctypes.c_uint64)
    def _on_target_completed(context, path, bytes_written):
        print("Target completed")

    @staticmethod
    @ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.c_uint16, ctypes.c_uint64)
    def _on_overwrite_completed(context, passes, bytes_written):
        print("Overwrite completed")
        Kuura.keep_running = False

    def __del__(self):
        Kuura.keep_running = False

        if Kuura._update_thread:
            Kuura._update_thread.join()

        if self.kuura_handle:
            self.kuura_free_fn(self.kuura_handle)

        self.kuura_api = None

    def add_target(self, target_type, target_path, delete_after):
        assert(self.kuura_handle)
        self.kuura_add_target_fn(self.kuura_handle, target_type, target_path.encode('utf-8'), delete_after)

    def add_pass(self, pass_type, verify_pass, optional=None):
        assert(self.kuura_handle)

        if optional:
            self.kuura_add_pass_fn(self.kuura_handle, pass_type, verify_pass, optional.encode('utf-8'))
        else:
            self.kuura_add_pass_fn(self.kuura_handle, pass_type, verify_pass, None)

    def run(self):
        assert(self.kuura_handle)
        Kuura.keep_running = True
        self.kuura_run_fn(self.kuura_handle)

    def version(self)->str:
        return self.kuura_version_fn().decode('ascii')

if __name__ == "__main__":
    this_directory =  os.path.dirname(os.path.abspath(__file__))
    kuura_api_path = None

    if sys.platform != "win32":
        kuura_api_path = this_directory + "/libkuura-api.so"
    else:
        kuura_api_path = this_directory + "\\KuuraAPI.dll"

    kuura = Kuura(kuura_api_path)
    print(kuura.version())

    signal.signal(signal.SIGINT, Kuura.signal_handler)

    kuura.add_target(target_type=Kuura.TargetType.DriveOverwrite, target_path="/dev/da0", delete_after=False)
    kuura.add_pass(pass_type=Kuura.FillType.ZeroFill, verify_pass=False)
    kuura.run()
