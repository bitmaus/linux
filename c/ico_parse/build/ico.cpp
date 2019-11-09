/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2017, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/
/* <DESC>
 * Get a web page, extract the title with libxml.
 * </DESC>

 Written by Lars Nilsson

 GNU C++ compile command line suggestion (edit paths accordingly):

 g++ -Wall -I/opt/curl/include -I/opt/libxml/include/libxml2 htmltitle.cpp \
 -o htmltitle -L/opt/curl/lib -L/opt/libxml/lib -lcurl -lxml2
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <curl/curl.h>
#include <libxml/HTMLparser.h>

#pragma comment(lib, "tidy.lib")
#pragma comment(lib, "LIBCURL.LIB")
#pragma comment(lib, "libxml2.lib")

//
//  Case-insensitive string comparison
//

#ifdef _MSC_VER
#define COMPARE(a, b) (!_stricmp((a), (b)))
#else
#define COMPARE(a, b) (!strcasecmp((a), (b)))
#endif

//
//  libxml callback context structure
//

struct Context
{
  Context(): addTitle(false) { }

  bool addTitle;
  std::string title;
};

//
//  libcurl variables for error strings and returned data

static char errorBuffer[CURL_ERROR_SIZE];
static std::string buffer;

//
//  libcurl write callback function
//

static int writer(char *data, size_t size, size_t nmemb, std::string *writerData)
{
  if(writerData == NULL)
    return 0;

  writerData->append(data, size*nmemb);

  return size * nmemb;
}

//
//  libcurl connection initialization
//

static bool init(CURL *&conn, char *url)
{
  CURLcode code;

  conn = curl_easy_init();

  if(conn == NULL) {
    fprintf(stderr, "Failed to create CURL connection\n");
    exit(EXIT_FAILURE);
  }

  code = curl_easy_setopt(conn, CURLOPT_ERRORBUFFER, errorBuffer);
  if(code != CURLE_OK) {
    fprintf(stderr, "Failed to set error buffer [%d]\n", code);
    return false;
  }

  code = curl_easy_setopt(conn, CURLOPT_URL, url);
  if(code != CURLE_OK) {
    fprintf(stderr, "Failed to set URL [%s]\n", errorBuffer);
    return false;
  }

  code = curl_easy_setopt(conn, CURLOPT_FOLLOWLOCATION, 1L);
  if(code != CURLE_OK) {
    fprintf(stderr, "Failed to set redirect option [%s]\n", errorBuffer);
    return false;
  }

  code = curl_easy_setopt(conn, CURLOPT_WRITEFUNCTION, writer);
  if(code != CURLE_OK) {
    fprintf(stderr, "Failed to set writer [%s]\n", errorBuffer);
    return false;
  }

  code = curl_easy_setopt(conn, CURLOPT_WRITEDATA, &buffer);
  if(code != CURLE_OK) {
    fprintf(stderr, "Failed to set write data [%s]\n", errorBuffer);
    return false;
  }

  return true;
}

//
//  libxml start element callback function
//

static void StartElement(void *voidContext,
                         const xmlChar *name,
                         const xmlChar **attributes)
{
  Context *context = (Context *)voidContext;
  printf((char *)name);
  if(COMPARE((char *)name, "LINK")) {
    printf("found");
    context->title = "found";
    context->addTitle = true;
  }
  (void) attributes;
}

//
//  libxml end element callback function
//

static void EndElement(void *voidContext,
                       const xmlChar *name)
{
  Context *context = (Context *)voidContext;

  if(COMPARE((char *)name, "LINK"))
    context->addTitle = false;
}

//
//  Text handling helper function
//

static void handleCharacters(Context *context,
                             const xmlChar *chars,
                             int length)
{
  if(context->addTitle)
    context->title.append((char *)chars, length);
}

//
//  libxml PCDATA callback function
//

static void Characters(void *voidContext,
                       const xmlChar *chars,
                       int length)
{
  Context *context = (Context *)voidContext;

  handleCharacters(context, chars, length);
}

//
//  libxml CDATA callback function
//

static void cdata(void *voidContext,
                  const xmlChar *chars,
                  int length)
{
  Context *context = (Context *)voidContext;

  handleCharacters(context, chars, length);
}

//
//  libxml SAX callback structure
//

static htmlSAXHandler saxHandler =
{
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  StartElement,
  EndElement,
  NULL,
  Characters,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  cdata,
  NULL
};

//
//  Parse given (assumed to be) HTML text and return the title
//

static void parseHtml(const std::string &html, std::string &title)
{
  htmlParserCtxtPtr ctxt;
  Context context;

  ctxt = htmlCreatePushParserCtxt(&saxHandler, &context, "", 0, "",
                                  XML_CHAR_ENCODING_NONE);

  htmlParseChunk(ctxt, html.c_str(), html.size(), 0);
  htmlParseChunk(ctxt, "", 0, 1);

  htmlFreeParserCtxt(ctxt);

  title = context.title;
}

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t written;
    written = fwrite(ptr, size, nmemb, stream);
    return written;
}


void traverse_dom_trees(xmlNode * a_node)
{
    xmlNode *cur_node = NULL;

    if(NULL == a_node)
    {
        //printf("Invalid argument a_node %p\n", a_node);
        return;
    }

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) 
    {
        if (cur_node->type == XML_ELEMENT_NODE) 
        {
          if (COMPARE((char *)cur_node->name, "LINK"))
          //if (cur_node->name == "link")
            /* Check for if current node should be exclude or not */
            {
            printf("Node type: Text, name: %s\n", cur_node->name);
            //printf("node type: Text, node content: %s,  content length %d\n", (char *)cur_node->content, strlen((char *)cur_node->content));
            printf("node: %s\n", cur_node->properties[0].name);
            printf("node: %s\n", cur_node->properties[0].children->content);
            CURL *curl;

            curl = curl_easy_init();
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, FALSE);
            FILE *file = fopen("headers.ico", "wb");
            //curl_easy_setopt(easy, CURLOPT_HEADERDATA, file);
            curl_easy_setopt(curl, CURLOPT_URL, cur_node->properties[0].children->content);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1); //Prevent "longjmp causes uninitialized stack frame" bug
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "deflate");
    //std::stringstream out;
            printf("1");
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
            printf("2");
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
            printf("3");
            //curl_easy_perform(curl);
            printf("4");
            

    //curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);
    /* Perform the request, res will get the return code */
    CURLcode res = curl_easy_perform(curl);
    /* Check for errors */
    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));}
                fclose(file);
            }
        }
        else if(cur_node->type == XML_TEXT_NODE)
        {
            /* Process here text node, It is available in cpStr :TODO: */
            //printf("node type: Text, node content: %s,  content length %d\n", (char *)cur_node->content, strlen((char *)cur_node->content));
        }
        traverse_dom_trees(cur_node->children);
    }
}

int main(int argc, char *argv[])
{
  CURL *conn = NULL;
  CURLcode code;
  std::string title;

  // Ensure one argument is given

  if(argc != 2) {
    fprintf(stderr, "Usage: %s <url>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  curl_global_init(CURL_GLOBAL_DEFAULT);

  // Initialize CURL connection

  if(!init(conn, argv[1])) {
    fprintf(stderr, "Connection initializion failed\n");
    exit(EXIT_FAILURE);
  }

  // Retrieve content for the URL

  code = curl_easy_perform(conn);
  curl_easy_cleanup(conn);

  if(code != CURLE_OK) {
    fprintf(stderr, "Failed to get '%s' [%s]\n", argv[1], errorBuffer);
    exit(EXIT_FAILURE);
  }
  printf("starting");

  // Parse the (assumed) HTML code
  parseHtml(buffer, title);

  // Display the extracted title
  printf("Title: %s\n", title.c_str());

  htmlDocPtr doc;
  xmlNode *roo_element = NULL;

  doc = htmlReadFile("test.htm", NULL, HTML_PARSE_NOBLANKS | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING | HTML_PARSE_NONET);

  roo_element = xmlDocGetRootElement(doc);

    if (roo_element == NULL) 
    {
        fprintf(stderr, "empty document\n");
        xmlFreeDoc(doc);
        return 0;
    }

    printf("Root Node is %s\n", roo_element->name);
    traverse_dom_trees(roo_element);

    xmlFreeDoc(doc);       // free document
    xmlCleanupParser();    // Free globals

  return EXIT_SUCCESS;
}
