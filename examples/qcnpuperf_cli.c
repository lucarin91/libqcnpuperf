/*
    Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
    Redistribution and use in source and binary forms, with or without
    modification, are permitted (subject to the limitations in the
    disclaimer below) provided that the following conditions are met:
        * Redistributions of source code must retain the above copyright
          notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above
          copyright notice, this list of conditions and the following
          disclaimer in the documentation and/or other materials provided
          with the distribution.
        * Neither the name of Qualcomm Technologies, Inc. nor the names of its
          contributors may be used to endorse or promote products derived
          from this software without specific prior written permission.
    NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
    GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
    HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
    WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
    MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
    GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
    IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
    IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "qcom_dsp.h"
#include <unistd.h>
#include <ncurses.h>
#include <stdio.h>

static const char *domain_to_str(enum DspDomainId domain)
{
	switch (domain) {
	case DSP_ADSP: return "ADSP";
	case DSP_NPU0: return "NPU0 (CDSP)";
	default:       return "UNKNOWN";
	}
}

/*TODO: Extend it to other DSPS */ 
int main(int argc, char *argv[])
{
	struct sysmon_query_prof_data *data;
	enum DspDomainId domain = DSP_NPU0;
	enum DspReturnCode ret;
	int no_metrics = 0;
	int use_tui = isatty(STDOUT_FILENO);

	if (use_tui) {
		initscr();
		noecho();
		curs_set(FALSE);
	}

	ret = qcom_dsp_init(domain);
	if (ret != RETURN_CODE_DSP_LIB_SUCCESS) {
		if (use_tui) endwin();
		fprintf(stderr, "qcom_dsp_init failed, ret=%d\n", ret);
		return EXIT_FAILURE;
	}

	while(true) {
		data = qcom_dsp_get_prof_data(domain, &no_metrics);
		if (!data || no_metrics <= 0) {
			if (use_tui) endwin();
		    fprintf(stderr, "qcom_dsp_get_prof_data failed\n");
		    qcom_dsp_deinit(domain);
		    return EXIT_FAILURE;
		}

		if (use_tui) {
			mvprintw(0, 0, "----------------- %s Stats---------------------\n", domain_to_str(domain));
			mvprintw(1, 0, "Q6 Utilization        : %.2f %%\n", data->q6_utilization);
			mvprintw(2, 0, "Q6 Clock              : %u KHz\n", data->q6_clock);
			mvprintw(3, 0, "HVX Utilization       : %.2f %%\n", data->hvx_utilization);
			mvprintw(4, 0, "HMX Utiliziation       : %.2f %%\n", data->hmx_utilization);
			mvprintw(6, 0, "-------------------------------------------------\n");
			refresh();
		} else {
			printf("%s q6_utilization=%.2f q6_clock=%u hvx_utilization=%.2f hmx_utilization=%.2f\n",
				domain_to_str(domain),
				data->q6_utilization,
				data->q6_clock,
				data->hvx_utilization,
				data->hmx_utilization);
			fflush(stdout);
		}
		sleep(1);
	}

	ret = qcom_dsp_deinit(domain);
	if (ret != RETURN_CODE_DSP_LIB_SUCCESS) {
	    fprintf(stderr, "qcom_dsp_deinit failed, ret=%d\n", ret);
	    return EXIT_FAILURE;
	}
	if (use_tui) endwin();
	return EXIT_SUCCESS;
}
