import ctypes
import os
import sys

KUURA_STRING = ctypes.c_char_p if sys.platform != "win32" else ctypes.c_wchar_p

class Kuura:
    class Handle(ctypes.Structure):
        _fields_ = [
            ("Unused", ctypes.c_int32),
        ]

    def __init__(self):
        kuura_api_path = os.path.dirname(os.path.abspath(__file__)) + "/KuuraAPI.dll"
        kuura_api = ctypes.CDLL(kuura_api_path)

        self.kuura_init_fn = kuura_api.KuuraInitialize
        self.kuura_init_fn.restype = ctypes.POINTER(Kuura.Handle)
        self.kuura_init_fn.argtypes = [ctypes.c_void_p]

        self.kuura_add_target_fn = kuura_api.KuuraAddTarget
        self.kuura_add_target_fn.restype = ctypes.c_bool
        self.kuura_add_target_fn.argtypes = [ctypes.POINTER(Kuura.Handle), ctypes.c_char, KUURA_STRING, ctypes.c_bool]

        self.kuura_add_pass_fn = kuura_api.KuuraAddPass
        self.kuura_add_pass_fn.restype = ctypes.c_bool
        self.kuura_add_target_fn.argtypes = [ctypes.POINTER(Kuura.Handle), ctypes.c_char, ctypes.c_bool, KUURA_STRING]

        self.kuura_set_overwrite_stared_fn = kuura_api.KuuraSetOverwriteStartedCallback
        kuura_overwrite_started_cb = ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.c_uint16, ctypes.c_uint64)
        self.kuura_set_overwrite_stared_fn.argtypes = [ctypes.POINTER(Kuura.Handle), kuura_overwrite_started_cb]

        self.kuura_set_target_started_fn = kuura_api.KuuraSetTargetStartedCallback
        kuura_target_started_cb = ctypes.CFUNCTYPE(None, ctypes.c_void_p, KUURA_STRING, ctypes.c_uint64)
        self.kuura_set_target_started_fn.argtypes = [ctypes.POINTER(Kuura.Handle), kuura_target_started_cb]

        self.kuura_set_pass_started_fn = kuura_api.KuuraSetPassStartedCallback
        kuura_pass_started_cb = ctypes.CFUNCTYPE(None, ctypes.c_void_p, KUURA_STRING, ctypes.c_uint16)
        self.kuura_set_pass_started_fn.argtypes = [ctypes.POINTER(Kuura.Handle), kuura_pass_started_cb]

        self.kuura_set_progress_fn = kuura_api.KuuraSetProgressCallback
        kuura_progress_cb = ctypes.CFUNCTYPE(ctypes.c_bool, ctypes.c_void_p, KUURA_STRING, ctypes.c_uint16, ctypes.c_uint64)
        self.kuura_set_progress_fn.argtypes = [ctypes.POINTER(Kuura.Handle), kuura_progress_cb]

        self.kuura_set_pass_completed_fn = kuura_api.KuuraSetPassCompletedCallback
        kuura_pass_completed_cb = ctypes.CFUNCTYPE(None, ctypes.c_void_p, KUURA_STRING, ctypes.c_uint16)
        self.kuura_set_pass_completed_fn.argtypes = [ctypes.POINTER(Kuura.Handle), kuura_pass_completed_cb]

        self.kuura_set_target_completed_fn = kuura_api.KuuraSetTargetCompletedCallback
        kuura_target_completed_cb = ctypes.CFUNCTYPE(None, ctypes.c_void_p, KUURA_STRING, ctypes.c_uint64)
        self.kuura_set_target_completed_fn = [ctypes.POINTER(Kuura.Handle), kuura_target_completed_cb]

        self.kuura_set_overwrite_completed_fn = kuura_api.KuuraSetOverwriteCompletedCallback
        kuura_overwrite_completed_cb = ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.c_uint16, ctypes.c_uint64)
        self.kuura_set_overwrite_completed_fn = [ctypes.POINTER(Kuura.Handle), kuura_overwrite_completed_cb]

        self.kuura_set_error_cb = kuura_api.KuuraSetErrorCallback
        kuura_error_cb = ctypes.CFUNCTYPE(None, ctypes.c_void_p, KUURA_STRING, ctypes.c_char, ctypes.c_uint16, ctypes.c_uint64, ctypes.c_uint32)
        self.kuura_set_error_cb.argtypes = [ctypes.POINTER(Kuura.Handle), kuura_error_cb]

        self.kuura_run_fn = kuura_api.KuuraRun
        self.kuura_run_fn.restype = ctypes.c_bool
        self.kuura_run_fn.argtypes = [ctypes.POINTER(Kuura.Handle)]

        self.kuura_free_fn = kuura_api.KuuraFree
        self.kuura_free_fn.argtypes = [ctypes.POINTER(Kuura.Handle)]

        self.kuura_version_fn = kuura_api.KuuraVersion
        self.kuura_version_fn.restype = ctypes.c_char_p

        this = ctypes.cast(ctypes.pointer(ctypes.py_object(self)), ctypes.c_void_p)
        self.kuura_handle = self.kuura_init_fn(this)

    def __del__(self):
        if self.kuura_handle != None:
            self.kuura_free_fn(self.kuura_handle)

    def version(self)->str:
        return self.kuura_version_fn().decode('ascii')

if __name__ == "__main__":
    kuura = Kuura()
    print(kuura.version())