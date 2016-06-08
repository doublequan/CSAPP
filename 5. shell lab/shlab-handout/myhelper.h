/***********************
 * my helper routines
 ***********************/
void unix_error(char *msg);

pid_t Fork() {
    pid_t pid;
    if ((pid = fork()) < 0)
	unix_error("Fork error");
    return pid;
}

void Kill(pid_t pid, int sig) {
    if (kill(pid, sig) < 0)
	unix_error("Kill error");
}
