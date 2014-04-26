/*
	simple API for SkyNet Internet of Things 
	25-Apr-2014  Ted Hale  created

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <curl/curl.h>

#define SKYNET_URL "http://skynet.im/devices/"

struct url_data {
	size_t size;
	char* data;
};

/****************************************************************************/
// callback for data returned
size_t write_data(void *ptr, size_t size, size_t nmemb, struct url_data *data) {
	size_t index = data->size;
	size_t n = (size * nmemb);
	char* tmp;

	data->size += (size * nmemb);
	tmp = realloc(data->data, data->size + 1); /* +1 for '\0' */

	if(tmp) {
		data->data = tmp;
	} else {
		if(data->data) {
			free(data->data);
		}
		printf("skynet_put Failed to allocate memory.\n");
		return 0;
	}

	memcpy((data->data + index), ptr, n);
	data->data[data->size] = '\0';

	return size * nmemb;
}

/****************************************************************************/
// update data for device
void skynet_put(char *uuid, char* token, char *data)
{
	CURL		*curl;
	CURLcode	res;
	char 		buff[1000];
	char 		url[1000];
	struct 		url_data response;
	
	response.size = 0;
	response.data = malloc(4096); /* reasonable size initial buffer */
	if(NULL == response.data) {
		printf("skynet_post Failed to allocate memory.\n");
		return;
	}

	response.data[0] = '\0';
	curl = curl_easy_init();
	if(curl) {
		sprintf(buff,"token=%s&%s}",token,data);
		strcpy(url,SKYNET_URL);
		strcat(url,uuid);
		///printf("url: %s\n",url);
		///printf("buff: %s\n",buff);
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buff);	
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
		res = curl_easy_perform(curl);
		if(res != CURLE_OK)
		  fprintf(stderr, "curl_easy_perform() failed: %s\n",
				  curl_easy_strerror(res));
		curl_easy_cleanup(curl);
	}
	///printf("RESPONSE:\n%s\n",response.data);
	free (response.data);
	return;
}

/****************************************************************************/
// read data for device
char *skynet_get(char *uuid)
{
	CURL		*curl;
	CURLcode	res;
	char 		url[1000];
	struct 		url_data response;
	
	response.size = 0;
	response.data = malloc(4096); /* reasonable size initial buffer */
	if(NULL == response.data) {
		printf("skynet_post Failed to allocate memory.\n");
		return;
	}

	response.data[0] = '\0';
	curl = curl_easy_init();
	if(curl) {
		strcpy(url,SKYNET_URL);
		strcat(url,uuid);
		///printf("url: %s\n",url);
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");	
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
		res = curl_easy_perform(curl);
		if(res != CURLE_OK)
		  fprintf(stderr, "curl_easy_perform() failed: %s\n",
				  curl_easy_strerror(res));
		curl_easy_cleanup(curl);
	}
	///printf("RESPONSE:\n%s",response.data);
	return response.data;
}

/****************************************************************************/
/*
int main(int argc, char **argv)
{
	char *resp;
	
	skynet_put("a9b94b21-c0ea-11e3-887f-2fb56b40c627",
				"0fdoa22ly9bxzuxr91tucn5it6luwhfr",
				"current_temp=99.123");

	resp = skynet_get("a9b94b21-c0ea-11e3-887f-2fb56b40c627");
	if (!resp) return 0;
	printf("GOT: %s\n",resp);
	
	skynet_put("a9b94b21-c0ea-11e3-887f-2fb56b40c627",
				"0fdoa22ly9bxzuxr91tucn5it6luwhfr",
				"current_temp=123.45");

	resp = skynet_get("a9b94b21-c0ea-11e3-887f-2fb56b40c627");
	if (!resp) return 0;
	printf("GOT: %s\n",resp);

	return 0;
}
*/
