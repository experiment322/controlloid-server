import subprocess
import platform

WS_PORT = 1337


def start():
    if platform.system() == 'Linux':
        subprocess.run(
            ['./linux/websocketd', '--binary', '--port', str(WS_PORT), 'python', './linux/ws_handler.py']
        )
    else:
        print(platform.system() + ' is not supported')


if __name__ == '__main__':
    try:
        start()
    except KeyboardInterrupt:
        pass
