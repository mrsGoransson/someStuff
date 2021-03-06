import time


class CTimer:
    def __init__(self):
        self._start_time = None
        self._delta_time = None
        self._elapsed_time = None
        self._last_time = None

    @staticmethod
    def get_current_time():
        return time.time()

    @property
    def get_elapsed_time(self):
        return self._elapsed_time

    @property
    def get_frame_time(self):
        return self._delta_time

    def start(self):
        if self._start_time is not None:
            print("Error, timer started but is already running")
            return

        self._start_time = time.time()
        self._last_time = self._start_time

    def update(self):
        new_time = time.time()
        self._elapsed_time = new_time - self._start_time
        self._delta_time = new_time - self._last_time
        if self._delta_time < 0.017:
            time.sleep(0.017-self._delta_time)
        self._delta_time = time.time() - self._last_time
        self._last_time = new_time
