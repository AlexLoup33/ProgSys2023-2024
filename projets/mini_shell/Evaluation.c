#include "Evaluation.h"

#include "Shell.h"

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#define _XOPEN_SOURCE 700

int run(Expression *e) {
    pid_t pid = fork();
    if (pid == 0) {
        execvp(e->argsList.args[0], e->argsList.args);
        exit(EXIT_FAILURE);
    }
    int stats;
    waitpid(pid, &stats, 0);
    if (WIFEXITED(stats)) {
        return WEXITSTATUS(stats);
    }
    return WTERMSIG(stats) + 128;
}

int run_pipe(Expression *gauche, Expression *droite) {
    enum { R,
           W };

    int fd[2];
    pipe(fd);

    pid_t pid1 = fork();
    if (pid1 == 0) {
        close(fd[R]);
        dup2(fd[W], STDOUT_FILENO);
        close(fd[W]);
        int out = evaluateExpr(gauche);
        exit(out);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        close(fd[W]);
        dup2(fd[R], STDIN_FILENO);
        close(fd[R]);
        int out = evaluateExpr(droite);
        exit(out);
    }

    close(fd[R]);
    close(fd[W]);

    int stats1, stats2;
    waitpid(pid1, &stats1, 0);
    waitpid(pid2, &stats2, 0);
    if (WIFEXITED(stats1)) {
        return WEXITSTATUS(stats1);
    }
    return WEXITSTATUS(stats2);
}

void handler(int sig) {
    int status;
    pid_t pid;
    do {
        UNBLOCK, &sig, NULL;
    }while(pid > 0);
}

int run_background(Expression *expr) {
    struct sigaction sig;
    sigemptyset(&sig.sa_mask);
    sig.sa_flags = SA_RESTART;
    sig.sa_handler = handler;
    sigaction(SIGCHLD, &sig, NULL);

    pid_t pid = fork();
    if (pid == 0) {
        int out = evaluateExpr(expr->left);
        exit(out);
    }
    sigprocmask(SIG_UNBLOCK, &sig, NULL);
}

int evaluateExpr(Expression *expr) {
    if (expr->type == ET_EMPTY) {
        return EXIT_SUCCESS;
    } else if (expr->type == ET_SIMPLE) {
        shellStatus = run(expr);
    } else if (expr->type == ET_SEQUENCE) {
        shellStatus = evaluateExpr(expr->left);
        shellStatus = evaluateExpr(expr->right);
    } else if (expr->type == ET_SEQUENCE_AND) {
        shellStatus = evaluateExpr(expr->left);
        if (shellStatus == 0) {
            shellStatus = evaluateExpr(expr->right);
        }
    } else if (expr->type == ET_SEQUENCE_OR) {
        shellStatus = evaluateExpr(expr->left);
        if (shellStatus) {
            shellStatus = evaluateExpr(expr->right);
        }
    } else if (expr->type == ET_PIPE) {
        shellStatus = run_pipe(expr->left, expr->right);
    } else if (expr->type == ET_BG) {
        shellStatus = run(expr);
    } else if (expr->type == ET_REDIRECT) {
        int fd;
        if (expr->redirect.type == REDIR_IN) {
            fd = open(expr->redirect.fileName, O_RDONLY);
            int save = dup(STDIN_FILENO);
            dup2(fd, STDIN_FILENO);
            close(fd);
            shellStatus = evaluateExpr(expr->left);
            dup2(save, STDIN_FILENO);
            close(save);
        } else if (expr->redirect.type == REDIR_OUT) {
            fd = open(expr->redirect.fileName, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            int save = dup(STDOUT_FILENO);
            int save2 = dup(STDERR_FILENO);
            dup2(fd, STDOUT_FILENO);
            dup2(fd, STDERR_FILENO);
            close(fd);
            shellStatus = evaluateExpr(expr->left);
            dup2(save, STDOUT_FILENO);
            dup2(save2, STDERR_FILENO);
            close(save);
            close(save2);
        } else if (expr->redirect.type == REDIR_APP) {
            fd = open(expr->redirect.fileName, O_WRONLY | O_CREAT | O_APPEND, 0644);
            int save = dup(STDOUT_FILENO);
            dup2(fd, STDOUT_FILENO);
            close(fd);
            shellStatus = evaluateExpr(expr->left);
            dup2(save, STDOUT_FILENO);
            close(save);
        }
    }
    return shellStatus;
}
