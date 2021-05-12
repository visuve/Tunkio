import ctypes
import pathlib
import sys

class Kuura:
    class Handle(ctypes.Structure):
        _fields_ = [
            ("Unused", ctypes.c_int32),
        ]

    def __init__(self):
        kuura_api_path = pathlib.Path().absolute() / "KuuraAPI.dll"
        kuura_api = ctypes.CDLL(kuura_api_path)

        self.kuura_version_fn = kuura_api.KuuraVersion

        if sys.platform == "win32":
            self.kuura_version_fn.restype = ctypes.c_wchar_p
        else:
            self.kuura_version_fn.restype = ctypes.c_char_p

        self.kuura_free_fn = kuura_api.KuuraFree
        self.kuura_free_fn.argtypes = [ctypes.POINTER(Kuura.Handle)]

        self.kuura_init_fn = kuura_api.KuuraInitialize
        self.kuura_init_fn.restype = [ctypes.POINTER(Kuura.Handle)]
        self.kuura_init_fn.argtypes = [ctypes.c_void_p]

        self.kuura_handle = self.kuura_init_fn(self)

    def __del__(self):
        if self.kuura_handle != None:
            self.kuura_free_fn(self.kuura_handle)

    def version(self):
        address = self.kuura_version_fn()

        if sys.platform == 'win32':
            return ctypes.c_wchar_p.from_buffer(address)

        return ctypes.c_char_p.from_buffer(address)

if __name__ == "__main__":
    kuura = Kuura()
    print(kuura.version())