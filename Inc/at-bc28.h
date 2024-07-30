/*
 * NB-IoT_test.h
 *
 *  Created on: 2024年7月3日
 *      Author: 杨璐
 */

#ifndef INC_AT_BC28_H_
#define INC_AT_BC28_H_

#include "at_cmd.h"

enum
{
	STAT_INIT,
	STAT_PRESEND,
	STAT_CONF,
	STAT_RDY,
};

typedef struct nbiot_conf_s
{
	int		status;
	char	manufacturers[64];
	char	model[64];
	char	imei[64];
	char	sim[64];
	char	csq[64];
}nbiot_conf_t;

#define NCDP			"221.229.214.202,5683"

extern nbiot_conf_t		g_nbconf;

extern int nb_reset_ok(comport_t *comport);
extern int nb_hdw_ok(comport_t *comport);
extern int nb_conf_ok(comport_t *comport);
extern int bc28_check_at(comport_t *comport);
extern int bc28_reset(comport_t *comport);
extern int bc28_get_manuf(comport_t *comport, char *reply_buf, size_t size);
extern int bc28_get_module(comport_t *comport, char *reply_buf, size_t size);
extern int bc28_check_imei(comport_t *comport, char *reply_buf, size_t size);
extern int bc28_check_simcd(comport_t *comport, char *reply_buf, size_t size);
extern int bc28_set_autocnt(comport_t *comport);
extern int bc28_check_cfun(comport_t *comport, char *reply_buf, size_t size);
extern int bc28_check_csq(comport_t *comport, char *reply_buf, size_t size);
extern int bc28_set_attach_net(comport_t *comport);
extern int bc28_check_attach_net(comport_t *comport, char *reply_buf, size_t size);
extern int bc28_check_reg_status(comport_t *comport, char *reply_buf,size_t size);
extern int bc28_check_ip(comport_t *comport, char *reply_buf,size_t size);
extern int bc28_set_ip_port(comport_t *comport, char *reply_buf, size_t size);
extern int bc28_check_ip_port(comport_t *comport, char *reply_buf, size_t size);
extern int bc28_check_iot(comport_t *comport, char *reply_buf, size_t size);
#endif /* INC_AT_BC28_H_ */
