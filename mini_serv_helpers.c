/*
 * DESCRIPTION:
 * file that shows two functions that were pre-given by the subject itself
 * the descriptions of the functions were not pre-given but added by myself
 * students are not forced to use these functions
 */

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>


/**
 * PRE-GIVEN function
 * @brief extracts one complete line/message from a buffered stream.
 *
 * This helper splits @buf at the first '\n', returns the extracted part in
 * @msg, and keeps the remaining bytes in @buf. In mini_irc_webserv, this is
 * useful for handling TCP input incrementally, where a client message may
 * arrive in fragments or several commands may arrive in a single read.
 * The function lets the server process one IRC-style line at a time while
 * preserving any unfinished data for the next poll/read cycle.
 *
 * @param buf pointer to the buffer containing the incoming data stream. This buffer is modified in-place to remove the extracted message.
 * @param msg pointer to a char pointer where the extracted message will be stored. The caller is responsible for freeing this memory after use.
 * @return 1 if a complete message was successfully extracted, 0 if no complete message was found (i.e., no '\n' in the
 */
int extract_message(char **buf, char **msg)
{
	char *newbuf;
	int i;

	*msg = 0;
	if (!*buf)
		return (0);
	i = 0;
	while ((*buf)[i])
	{
		if ((*buf)[i] == '\n')
		{
			newbuf = calloc(1, sizeof(*newbuf) * (strlen(*buf + i + 1) + 1));
			if (!newbuf)
				return (-1);
			strcpy(newbuf, *buf + i + 1);
			*msg = *buf;
			(*msg)[i + 1] = 0;
			*buf = newbuf;
			return (1);
		}
		i += 1;
	}
	return (0);
}

/*
 * PRE-GIVEN function
 * @brief it adds another string (add) to an existing one (buf) or copies 
 * the other string to a new string with heap allocation and returns this new string
 *
 * free (NULL) is valid, it just does nothing
 *
 */
char *str_join(char *buf, char *add) {
    char *newbuf;
    int len;

    if (buf == 0)
        len = 0;
    else
        len = strlen(buf);
    newbuf = malloc(sizeof(*newbuf) * (len + strlen(add) + 1));
    if (newbuf == 0)
        return (0);
    newbuf[0] = 0;
    if (buf != 0)
        strcat(newbuf, buf);
    free(buf);
    strcat(newbuf, add);
    return (newbuf);
}
