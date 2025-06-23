#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

// Minimal stub implementations for newlib syscalls

int _close(int file) {
    errno = EBADF;
    return -1;
}

off_t _lseek(int file, off_t ptr, int dir) {
    errno = EBADF;
    return -1;
}

ssize_t _read(int file, void *ptr, size_t len) {
    errno = EBADF;
    return -1;
}

ssize_t _write(int file, const void *ptr, size_t len) {
    errno = EBADF;
    return -1;
}

int _fstat(int file, struct stat *st) {
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file) {
    return 1;
}

int _open(const char *name, int flags, int mode) {
    errno = ENOSYS;
    return -1;
}

void _exit(int status) {
    while (1) { }
}

int _kill(int pid, int sig) {
    errno = EINVAL;
    return -1;
}

int _getpid(void) {
    return 1;
}

void *_sbrk(ptrdiff_t incr) {
    extern char _end; // Defined by the linker script
    static char *heap_end;
    char *prev_heap_end;

    if (heap_end == 0) {
        heap_end = &_end;
    }
    prev_heap_end = heap_end;
    heap_end += incr;
    return (void *)prev_heap_end;
}
