from threading import Lock
from watchdog.observers import Observer
from watchdog.events import PatternMatchingEventHandler

from Timer import CTimer


class CFileWatcher:
    def __init__(self):
        self._event_handler = PatternMatchingEventHandler(patterns="*", ignore_directories=True)
        self._event_handler.on_deleted = self._on_deleted
        self._event_handler.on_modified = self._on_modified
        self._observer = Observer()
        self._observer.schedule(event_handler=self._event_handler, path='.')
        self._observer.start()
        self._lock = Lock()
        self._last_modified_threshold = 5.0
        self._paths_to_observe = {}
        self._listeners = {}

    def add_listener(self, event, source_path, callback):
        with self._lock:
            if event in self._listeners:
                self._listeners[event].append({'path': source_path, 'callback': callback, 'needs_update': False})
            else:
                self._listeners[event] = [{'path': source_path, 'callback': callback, 'needs_update': False}]

            if source_path not in self._paths_to_observe:
                self._paths_to_observe[source_path] = ({'last_modified': CTimer.get_current_time()})

    def _handle_on_delete_event(self, affected_source_path):
        self._handle_event(affected_source_path, 'on_deleted')

    def _handle_on_modify_event(self, affected_source_path):
        self._paths_to_observe[affected_source_path]['last_modified'] = CTimer.get_current_time()
        self._handle_event(affected_source_path, 'on_modified')

    def _handle_event(self, affected_source_path, event):
        with self._lock:
            for listener in self._listeners[event]:
                if affected_source_path == (listener['path']):
                    listener['callback']()

    def _has_time_passed_threshold(self, last_modified_at):
        return CTimer.get_current_time() > last_modified_at + self._last_modified_threshold

    def _on_deleted(self, event):
        print(event)
        affected_path = event.src_path.lstrip(".\\")
        with self._lock:
            if affected_path not in self._paths_to_observe:
                return
        self._handle_on_delete_event(affected_path)

    def _on_modified(self, event):
        affected_path = event.src_path.lstrip(".\\")
        with self._lock:
            if affected_path not in self._paths_to_observe:
                return
            can_modify = self._has_time_passed_threshold(self._paths_to_observe[affected_path]['last_modified'])
        if can_modify:
            self._handle_on_modify_event(affected_path)
