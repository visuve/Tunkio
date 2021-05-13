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
        # kuura_api_path = os.path.dirname(os.path.abspath(__file__)) + "/KuuraAPI.dll"
        kuura_api_path = "D:\\DEV\\kuura\\out\\build\\x64-Debug\\KuuraAPI.dll"
        self.kuura_api = ctypes.CDLL(kuura_api_path)

        kuura_init_fn = self.kuura_api.KuuraInitialize
        kuura_init_fn.restype = ctypes.POINTER(Kuura.Handle)
        kuura_init_fn.argtypes = [ctypes.c_void_p]

        self.kuura_add_target_fn = self.kuura_api.KuuraAddTarget
        self.kuura_add_target_fn.restype = ctypes.c_bool
        self.kuura_add_target_fn.argtypes = [ctypes.POINTER(Kuura.Handle), ctypes.c_int32, KUURA_STRING, ctypes.c_bool]

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
        '''
        kuura_add_pass_fn = kuura_api.KuuraAddPass
        kuura_add_pass_fn.restype = ctypes.c_bool
        kuura_add_target_fn.argtypes = [ctypes.POINTER(Kuura.Handle), ctypes.c_char, ctypes.c_bool, KUURA_STRING]

        kuura_set_overwrite_stared_fn = kuura_api.KuuraSetOverwriteStartedCallback
        kuura_overwrite_started_cb = ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.c_uint16, ctypes.c_uint64)
        kuura_set_overwrite_stared_fn.argtypes = [ctypes.POINTER(Kuura.Handle), kuura_overwrite_started_cb]

        kuura_set_target_started_fn = kuura_api.KuuraSetTargetStartedCallback
        kuura_target_started_cb = ctypes.CFUNCTYPE(None, ctypes.c_void_p, KUURA_STRING, ctypes.c_uint64)
        kuura_set_target_started_fn.argtypes = [ctypes.POINTER(Kuura.Handle), kuura_target_started_cb]

        kuura_set_pass_started_fn = kuura_api.KuuraSetPassStartedCallback
        kuura_pass_started_cb = ctypes.CFUNCTYPE(None, ctypes.c_void_p, KUURA_STRING, ctypes.c_uint16)
        kuura_set_pass_started_fn.argtypes = [ctypes.POINTER(Kuura.Handle), kuura_pass_started_cb]

        kuura_set_progress_fn = kuura_api.KuuraSetProgressCallback
        kuura_progress_cb = ctypes.CFUNCTYPE(ctypes.c_bool, ctypes.c_void_p, KUURA_STRING, ctypes.c_uint16, ctypes.c_uint64)
        kuura_set_progress_fn.argtypes = [ctypes.POINTER(Kuura.Handle), kuura_progress_cb]

        kuura_set_pass_completed_fn = kuura_api.KuuraSetPassCompletedCallback
        kuura_pass_completed_cb = ctypes.CFUNCTYPE(None, ctypes.c_void_p, KUURA_STRING, ctypes.c_uint16)
        kuura_set_pass_completed_fn.argtypes = [ctypes.POINTER(Kuura.Handle), kuura_pass_completed_cb]

        kuura_set_target_completed_fn = kuura_api.KuuraSetTargetCompletedCallback
        kuura_target_completed_cb = ctypes.CFUNCTYPE(None, ctypes.c_void_p, KUURA_STRING, ctypes.c_uint64)
        self.kuura_set_target_completed_fn = [ctypes.POINTER(Kuura.Handle), kuura_target_completed_cb]

        kuura_set_overwrite_completed_fn = kuura_api.KuuraSetOverwriteCompletedCallback
        kuura_overwrite_completed_cb = ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.c_uint16, ctypes.c_uint64)
        self.kuura_set_overwrite_completed_fn = [ctypes.POINTER(Kuura.Handle), kuura_overwrite_completed_cb]
        '''

        kuura_set_error_cb = kuura_api.KuuraSetErrorCallback
        kuura_set_error_cb(self.kuura_handle, Kuura._on_error)
        # kuura_error_cb = ctypes.CFUNCTYPE(None, ctypes.c_void_p, KUURA_STRING, ctypes.c_char, ctypes.c_uint16, ctypes.c_uint64, ctypes.c_uint32)
        # kuura_set_error_cb.argtypes = [ctypes.POINTER(Kuura.Handle), kuura_error_cb]


    @staticmethod
    @ctypes.CFUNCTYPE(None, ctypes.c_void_p, KUURA_STRING, ctypes.c_char, ctypes.c_uint16, ctypes.c_uint64, ctypes.c_uint32)
    def _on_error(context, path, phase, passes, bytes_written, error_code):
        print("foobar")

    def __del__(self):
        assert(self.kuura_handle)
        self.kuura_free_fn(self.kuura_handle)
        self.kuura_api = None

    def add_target(self, target_type: ctypes.c_int32, target_path: KUURA_STRING, delete_after: ctypes.c_bool):
        assert(self.kuura_handle)
        self.kuura_add_target_fn(self.kuura_handle, target_type, target_path, delete_after)

    def add_pass(self, pass_type: ctypes.c_char, verify_pass: ctypes.c_bool, optional: ctypes.c_char_p = None):
        assert(self.kuura_handle)
        self.kuura_add_pass_fn(self.kuura_handle, pass_type, verify_pass, optional)

    def run(self):
        assert(self.kuura_handle)
        self.kuura_run_fn(self.kuura_handle)

    def version(self)->str:
        return self.kuura_version_fn().decode('ascii')

if __name__ == "__main__":
    kuura = Kuura()
    print(kuura.version())

    kuura.add_target(target_type='D', target_path=u"\\\\.\\PHYSICALDRIVE9", delete_after=False)