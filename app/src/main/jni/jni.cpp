#include <string.h>
#include <jni.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <deque>
#include <verctor>
#include <sys/time.h>
using namespace std;

int main(int argc, char* argv[]);

static int forwardFromChild = -1;
static int forwardToChild = -1;

extern "C" JNIEXPORT void JNICALL Java_org_empyrn_darknight_engine_NativePipedProcess_startProcess(JNIEnv* env, jobject obj) {
    int fd1[2];
    int fd2[2];

    if(pipe(fd1) < 0 || pipe(fd2) < 0) {
        exit(1);
    }

    int childpid = fork();

    switch(childpid) {
        case -1:
            exit(1);
            break;
        case 0:
            close(fd1[1]);
            close(fd2[0]);
            close(0); dup(fd1[0]); close(fd1[0]);
            close(1); dup(fd2[1]); close(fd2[1]);
            close(2); dup(1);
            static char* argv[] = {"stockfish", NULL};
    	    nice(5);
   	        main(1, argv);
    	    _exit(0);
            break;
        default:
            close(fd1[0]);
    	    close(fd2[1]);
            forwardFromChild = fd2[0];
            forwardToChild = fd1[1];
	        fcntl(forwardFromChild, F_SETFL, O_NONBLOCK);
            break;
    }
}

static deque<char> inputBuffer;
static vector<char> lineBuffer;

static bool getNextChar(int & c, int timeoutMill) {
    if(inputBuffer.empty()) {
        fd_set readfds, writefds;
        FD_ZERO(&readfds);
        FD_SET(forwardFromChild, &readfds);

        struct timeval tv;
        tv.tv_sec = timeoutMill / 1000;
        tv.tv_usec = (timeoutMill % 1000) * 1000;

        int ret = select(forwardFromChild + 1, &readfds, NULL, NULL, &tv);
        if (ret < 0) {
            return false;
        }

        static char buf[4096];
        int len = read(forwardFromChild, &buf[0], sizeof(buf));

        for (int i = 0; i < len; i++) {
            inputBuffer.push_back(buf[i]);
        }
    }

    if(inputBuffer.empty()) {
        c = -1;
        return true;
    }

    c = inputBuffer.front();
    inputBuffer.pop_front();
    return true;
}

extern "C" JNIEXPORT jstring JNICALL Java_org_empyrn_darknight_engine_NativePipedProcess_readFromProcess(JNIEnv* env, jobject obj, jint timeoutMill) {
    struct timeval tv0, tv1;

    while(true) {
        int c;

        gettimeofday(&tv0, NULL);

        if(!getNextChar(c, timeoutMill)) {
            return 0;
        }

        gettimeofday(&tv1, NULL);

        int elapsedMillis = (tv1.tv_sec - tv0.tv_sec) * 1000 + (tv1.tv_usec - tv0.tv_usec) / 1000;

        if(elapsedMillis > 0) {
            timeoutMill -= elapsedMillis;
            if (timeoutMill < 0) {
                timeoutMill = 0;
            }
        }

        if(c == -1) {
            static char emptyString = 0;
            return (*env).NewStringUTF(&emptyString);
        }

        if(c == '\n' || (c == '\r')) {
            if (lineBuffer.size() > 0) {
                lineBuffer.push_back(0);
                jstring ret = (*env).NewStringUTF(&lineBuffer[0]);
                lineBuffer.clear();
                return ret;
            }
        } else {
            lineBuffer.push_back((char)c);
        }
    }
}

extern "C" JNIEXPORT void JNICALL Java_org_empyrn_darknight_engine_NativePipedProcess_writeToProcess(JNIEnv* env, jobject obj, jstring msg){
    const char* str = (*env).GetStringUTFChars(msg, NULL);

    if(str) {
        int len = strlen(str);
        int written = 0;

        while(written < len) {
            int n = write(forwardToChild, &str[written], len - written);

            if(n < 0) {
                break;
            }

            written += n;
        }

        (*env).ReleaseStringUTFChars(msg, str);
    }
}
