import signal

def handler(signum, frame):
        print('Starting MIDICube')
        exit(0)

signal.signal(signal.SIGALRM, handler)
signal.alarm(5)

print('Starting MIDICube in 5 seconds ...')
input('Press ENTER key to interrupt')
signal.alarm(0)
print('Interrupted')
exit(1)
