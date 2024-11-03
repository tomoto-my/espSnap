#ifndef HTTP_URLS_H_ 
#define HTTP_URLS_H_

#include <StreamString.h>

// #include <uri/UriBraces.h>
// #include <uri/UriRegex.h>

// any source file that includes this header file will be able to use this global variable
// extern void server();

String urldecode(String str);
String urlencode(String str);
unsigned char h2int(char c);


#endif /* HTTP_URLS_H_ */