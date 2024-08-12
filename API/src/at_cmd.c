/*
 * at_cmd.c
 *
 *  Created on: 2024年7月14日
 *      Author: 杨璐
 */
#include "at_cmd.h"
#define CONFIG_OS_STM32
//#define CONFIG_OS_LINUX


atcmd_t	g_atcmd;
int         g_send_event = 0;
int         g_leds_event = 0;

#ifdef CONFIG_OS_STM32

EventGroupHandle_t		Event_Handle;
SemaphoreHandle_t		xSemaphore;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (huart->Instance == USART3)
    {
    	HAL_UART_Receive_IT(comport.dev, &data, 1);
        if(xStreamBufferSpacesAvailable(xStreamBuffer)>0)
        {
        	xStreamBufferSendFromISR(xStreamBuffer, &data, 1, &xHigherPriorityTaskWoken);
        }
        if('\n' == data)
        {
        	xSemaphoreGiveFromISR(xSemaphore,&xHigherPriorityTaskWoken);
        }
    }
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

int atcmd_pars(char *buf)
{
	char		*ptr,*end;
	int			bytes = 0;
	size_t		i;

	if('\0' == g_atcmd.xAtCmd[0])
	{
		return 0;
	}

	for(i=0;i<strlen(g_atcmd.xAtCmd);i++)
	{
		if(g_atcmd.xAtCmd[i] == '\n')
		{
			g_atcmd.xAtCmd[i] = '\0';
			break;
		}
	}

	/*AT command echo on*/
	if(!(ptr = strstr(buf,g_atcmd.xAtCmd)))
	{
		return 0;
	}

	if((end = strstr(ptr,AT_OKSTR)))
	{
		end += strlen(AT_OKSTR);
		bytes = end - ptr;
	}
	else if((end = strstr(ptr,AT_ERRSTR)))
	{
		if((end = strstr(ptr,"\r\n")))
		{
			end+=strlen(AT_ERRSTR);
			bytes = end - ptr;
		}
	}

	if(bytes)
	{
		strncpy(g_atcmd.xAtCmdReply,ptr,bytes);
		printf("got AT reply:%s",g_atcmd.xAtCmdReply);
		xEventGroupSetBits(Event_Handle,Receive_Event);
		return 1;
	}
	return 0;
}

int parser_async_message(char *buf,char *keystr)
{
	char		*ptr,*end;
	int			bytes = 0;
	char		Asynbuf[128];

	if(!(ptr=strstr(buf,keystr)))
		return -1;

	if(!(end=strstr(ptr+strlen(keystr),"\r\n")))
		return -2;

	ptr = ptr+strlen(keystr);
	bytes = end-ptr;

	strncpy(Asynbuf,ptr,(size_t)bytes);
	printf("^^^^^^^^Asynbuf=%s\r\n",Asynbuf);

	if(strstr(Asynbuf,"0622B8") && strstr(Asynbuf,"0101"))
	{
		printf("led on\n");
		HAL_GPIO_WritePin(GPIOB, green_led_Pin, GPIO_PIN_RESET);
	}

	else if(strstr(Asynbuf,"0100")&& strstr(Asynbuf,"0622B8"))
	{
		printf("led off\n");
		HAL_GPIO_WritePin(GPIOB, green_led_Pin, GPIO_PIN_SET);
	}

	memset(Asynbuf,0,sizeof(Asynbuf));
	return 0;
}
#endif

int atcmd_send(comport_t *comport,char *at, uint32_t timeout,char *expect, char *error, char *reply,size_t size)
{
#ifdef CONFIG_OS_STM32
	EventBits_t		r_event;
#endif
	char			*ptr,*end;
	int				res = 0,rv;
	int				i,bytes;

	if(!comport || !at)
		return -1;

#ifdef CONFIG_OS_LINUX

	if( comport->dev <= 0 )
	{
		log_error("comport[%s] not opened\n");
		return -2;
	}

	tcflush(comport->dev, TCIOFLUSH);
#endif

	printf("send AT command:%s\r\n",at);
	snprintf(g_atcmd.xAtCmd,ATCMD_SIZE,"%s%s",at,AT_SUFFIX);
	memset(g_atcmd.xAtCmdReply,0,ATCMD_SIZE);

	if(comport_send(comport, g_atcmd.xAtCmd, strlen(g_atcmd.xAtCmd)) <0 )
	{
		printf("Send AT command failed\r\n");
#ifdef CONFIG_OS_LINUX
		log_error("send AT command \"%s\" to \"%s\" failed, rv=%d\n", at, comport->devname, rv);
#endif
		res = -2;
		goto out;
	}
	printf("Send AT command OK\r\n");

#ifdef	CONFIG_OS_STM32
	r_event = xEventGroupWaitBits(Event_Handle,Receive_Event,pdTRUE, pdFALSE, pdMS_TO_TICKS(timeout));
	if(!(r_event&Receive_Event))
	{
		res = -3;
		goto out;
	}

	ptr = strstr(g_atcmd.xAtCmdReply,at);
	if(!ptr)
	{
		res = -4;
		goto out;
	}

	ptr += strlen(at);
	if(NULL != (end=strstr(ptr,expect)))
	{
		if(reply && size>0)
		{
			end += strlen(AT_OKSTR);
			memset(reply, 0, size);
			strncpy(reply,ptr,(size_t)(end-ptr));
			printf("AT command %s got reply:%s",at,reply);
		}
		res = 0;
		goto out;
	}

	if(NULL != (end=strstr(ptr,error)))
	{
		res = -5;
		goto out;
	}

#elif (defined CONFIG_OS_LINUX)

	res = ATRES_TIMEOUT;
	memset( g_atcmd.xAtCmdReply, 0, ATCMD_REPLY_LEN );


	for(i=0; i<timeout/10; i++)
	{
		if( g_send_event != 1 )
		{
			usleep(1000);
			break;
		}

		if( !(strstr(g_rece_flags.send_event_buf, at)) )
		{
			log_error("Received is not send AT command.\n");
			return -8;
		}
		length = strlen(g_rece_flags.send_event_buf);
		if(length < 0)
		{
			log_error("send AT command \'%s\' to \'%s\' failed, rv=%d\n", at, comport->devname, rv);
			return -3;
		}


		if( expect && strstr(g_rece_flags.send_event_buf, expect) )
		{
			log_debug("send AT command \"%s\" and got reply \"OK\"\n", at);
			res = ATRES_EXPECT;
			break;
		}

		if( error && strstr(g_rece_flags.send_event_buf, error) )
		{
			log_debug("send AT command \"%s\" and got reply \"ERROR\"\n", at);
			res = ATRES_ERROR;
			break;
		}
	}

	if( length > 0 )
		log_trace("AT command reply:%s", g_rece_flags.send_event_buf);

	if( reply && size>0 )
	{
		bytes = strlen(g_rece_flags.send_event_buf)>size ? size : strlen(g_rece_flags.send_event_buf);
		memset(reply, 0, size);
		strncpy(reply, g_rece_flags.send_event_buf, length);

		log_debug("copy out AT command \"%s\" reply message: \n%s", at, reply);
		g_send_event = 0;
	}
#endif
out:
	memset(g_atcmd.xAtCmd,0,sizeof(g_atcmd.xAtCmd));
	memset(g_atcmd.xAtCmdReply,0,sizeof(g_atcmd.xAtCmdReply));
	return res;
}

int atcmd_check_OK(comport_t *comport, char *at, uint32_t timeout)
{
	int		rv = 0;

	if(!at || !comport)
	{
		printf("Input invalid arguments\r\n");
		return -1;
	}

	rv = atcmd_send(comport, at, timeout, AT_OKSTR, AT_ERRSTR, NULL, 0);
	if(rv<0)
	{
		return -1;
	}

	return 0;
}

int atcmd_check_value(comport_t *comport, char *at, uint32_t timeout, char *reply, size_t size)
{
	int			rv = 0,i=0;
	char		buf[ATBUF_SIZE];
	char		*ptr,*end;
	uint32_t	len;

	if(!comport || !at || !reply || size<=0)
	{
		rv = -1;
		goto out;
	}

	rv = atcmd_send(comport, at, timeout, AT_OKSTR, AT_ERRSTR, buf, sizeof(buf));
	if(rv)
	{
		goto out;
	}

	ptr = strchr(buf,'+');
	if(!ptr)
	{
		rv = -2;
		goto out;
	}
	ptr++;

	ptr = strchr(buf,':');
	if(!ptr)
	{
		rv = -3;
		goto out;
	}
	ptr++;

	end = strstr(ptr,"\r\nOK");
	if(!end)
	{
		rv = -4;
		goto out;
	}
	memset(reply,0,size);

	if(!(len=strlen(ptr)))
	{
		rv = -5;
		goto out;
	}

	while(ptr != end)
	{
		if(*ptr!='\"')
			reply[i++] = *ptr;
		ptr++;
		rv = 0;
		goto out;
	}

out:
	memset(buf,0,sizeof(buf));
	if(rv<0)
		return -1;
	else
		return 0;
}

