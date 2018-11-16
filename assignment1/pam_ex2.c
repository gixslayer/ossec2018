/* Define which PAM interfaces we provide */
#define PAM_SM_ACCOUNT
#define PAM_SM_AUTH
#define PAM_SM_PASSWORD
#define PAM_SM_SESSION

/* Include PAM headers */
#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NUM_QUESTIONS 5
typedef struct {
	const char *prompt;
	const char *answer;
} question_t;

question_t questions[NUM_QUESTIONS];

/* PAM entry point for session creation */
int pam_sm_open_session(pam_handle_t *pamh, int flags, int argc, const char **argv) {
	return(PAM_IGNORE);
}

/* PAM entry point for session cleanup */
int pam_sm_close_session(pam_handle_t *pamh, int flags, int argc, const char **argv) {
	return(PAM_IGNORE);
}

/* PAM entry point for accounting */
int pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, int argc, const char **argv) {
	return(PAM_IGNORE);
}

/* PAM entry point for authentication verification */
int pam_sm_authenticate(pam_handle_t *pamh, int flags, int argc, const char **argv) {
	if(argc < 2 * NUM_QUESTIONS) {
		return PAM_IGNORE;
	}

	// extract questions and answers from arguments
	for(int i = 0; i < NUM_QUESTIONS; ++i) {
		questions[i].prompt = argv[i*2];
		questions[i].answer = argv[i*2+1];
	}

	// pick a random question
	srand(time(NULL));
	int q_i = rand() % NUM_QUESTIONS;

	char *line = NULL;
	size_t n = 0;
	int ret = PAM_AUTH_ERR;
	ssize_t len = 0;

	puts(questions[q_i].prompt);
	//puts(questions[q_i].answer);
	
	// get user input (strip tailing newline) and check if correct
	if((len = getline(&line, &n, stdin)) != -1) {
		if(len > 0 && line[len-1] == '\n') line[len-1] = 0;

		if(!strcmp(line, questions[q_i].answer)) {
			ret = PAM_SUCCESS;
		}
	}

	// since getline allocated line, free it now
	free(line);

	puts(ret == PAM_SUCCESS ? "PAM_EX2: Passed" : "PAM_EX2: Failed");

	return ret;
}

/*
   PAM entry point for setting user credentials (that is, to actually
   establish the authenticated user's credentials to the service provider)
 */
int pam_sm_setcred(pam_handle_t *pamh, int flags, int argc, const char **argv) {
	return(PAM_IGNORE);
}

/* PAM entry point for authentication token (password) changes */
int pam_sm_chauthtok(pam_handle_t *pamh, int flags, int argc, const char **argv) {
	return(PAM_IGNORE);
}
