// #include <mycss/entry.h>
#ifndef MyHTML_MyCSS_ENTRY_H
#define MyHTML_MyCSS_ENTRY_H
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "mycss/myosi.h"
#include "mycss/mycss.h"
#include "mycss/parser.h"
#include "mycss/stylesheet.h"
#include "mycss/namespace/myosi.h"
#include "mycss/namespace/init.h"
#include "mycss/selectors/myosi.h"
#include "mycss/selectors/init.h"
#include "mycss/an_plus_b.h"
#include "mycss/declaration/myosi.h"
#include "mycss/declaration/init.h"
#include "mycss/declaration/entry.h"
#include "mycss/media/myosi.h"
#include "mycss/media/init.h"
#include "myhtml/utils/mcobject.h"
#include "myhtml/utils/mchar_async.h"

struct mycss_entry_parser_list_entry {
    mycss_parser_token_f parser;
    mycss_parser_token_f parser_switch;
    mycss_token_type_t   ending_token;
    
    bool is_local;
}
typedef mycss_entry_parser_list_entry_t;

struct mycss_entry_parser_list {
    mycss_entry_parser_list_entry_t* list;
    size_t size;
    size_t length;
}
typedef mycss_entry_parser_list_t;

struct mycss_entry {
    /* refs */
    mycss_t* mycss;
    mycss_token_t* token;
    mycss_stylesheet_t* stylesheet;
    
    /* objects and memory for css modules */
    mchar_async_t* mchar;
    size_t mchar_node_id;
    size_t mchar_value_node_id;
    
    mcobject_t* mcobject_string_entries;
    
    /* css modules */
    mycss_namespace_t*   ns;
    mycss_selectors_t*   selectors;
    mycss_an_plus_b_t*   anb;
    mycss_media_t*       media;
//    mycss_rules_t*       rules;
    mycss_declaration_t* declaration;
    void**               values;
    
    /* incoming buffer */
    mcobject_t* mcobject_incoming_buffer;
    myhtml_incoming_buffer_t* first_buffer;
    myhtml_incoming_buffer_t* current_buffer;
    
    /* options */
    mycss_entry_type_t type;
    myhtml_encoding_t encoding;
    
    /* tokenizer */
    mycss_tokenizer_state_t state;
    mycss_tokenizer_state_t state_back;
    
    /* parser */
    mycss_entry_parser_list_t* parser_list;
    mycss_parser_token_f parser;
    mycss_parser_token_f parser_switch;
    mycss_parser_token_f parser_error;
    mycss_parser_token_f parser_original;
    mycss_token_type_t   parser_ending_token;
    
    /* callbacks */
    mycss_token_ready_callback_f token_ready_callback;
    mycss_callback_selector_done_f callback_selector_done;
    
    /* helpers */
    size_t token_counter;
    size_t help_counter;
};

mycss_entry_t * mycss_entry_create(void);
mycss_status_t mycss_entry_init(mycss_t* mycss, mycss_entry_t* entry);
mycss_status_t mycss_entry_clean(mycss_entry_t* entry);
mycss_status_t mycss_entry_clean_all(mycss_entry_t* entry);
mycss_entry_t * mycss_entry_destroy(mycss_entry_t* entry, bool self_destroy);

void mycss_entry_end(mycss_entry_t* entry);

/* api */
mycss_selectors_t * mycss_entry_selectors(mycss_entry_t* entry);

mycss_token_ready_callback_f mycss_entry_token_ready_callback(mycss_entry_t* entry, mycss_token_ready_callback_f callback_f);

size_t mycss_entry_token_count(mycss_entry_t* entry);
myhtml_incoming_buffer_t * mycss_entry_incoming_buffer_current(mycss_entry_t* entry);
myhtml_incoming_buffer_t * mycss_entry_incoming_buffer_first(mycss_entry_t* entry);

myhtml_string_t * mycss_entry_string_create_and_init(mycss_entry_t* entry, size_t string_size);

mycss_stylesheet_t * mycss_entry_stylesheet(mycss_entry_t* entry);
mycss_selectors_list_t * mycss_entry_current_selectors_list(mycss_entry_t* entry);

void mycss_entry_parser_set(mycss_entry_t* entry, mycss_parser_token_f parser);
void mycss_entry_parser_switch_set(mycss_entry_t* entry, mycss_parser_token_f parser_switch);
void mycss_entry_parser_original_set(mycss_entry_t* entry, mycss_parser_token_f parser_original);

/* parser list */
mycss_entry_parser_list_t * mycss_entry_parser_list_create_and_init(size_t size);
void mycss_entry_parser_list_clean(mycss_entry_parser_list_t* parser_list);
mycss_entry_parser_list_t * mycss_entry_parser_list_destroy(mycss_entry_parser_list_t* parser_list, bool self_destroy);

mycss_status_t mycss_entry_parser_list_push(mycss_entry_t* entry, mycss_parser_token_f parser_func,
                                            mycss_parser_token_f parser_switch, mycss_token_type_t ending_token,
                                            bool is_local);

void mycss_entry_parser_list_pop(mycss_entry_t* entry);
size_t mycss_entry_parser_list_length(mycss_entry_t* entry);
mycss_token_type_t mycss_entry_parser_list_current_ending_token_type(mycss_entry_t* entry);
mycss_parser_token_f mycss_entry_parser_list_current_parser(mycss_entry_t* entry);
mycss_parser_token_f mycss_entry_parser_list_current_parser_switch(mycss_entry_t* entry);
bool mycss_entry_parser_list_current_is_local(mycss_entry_t* entry);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MyHTML_MyCSS_ENTRY_H */
#include "mycss/selectors/function_resource.h"

mycss_entry_t * mycss_entry_create(void)
{
    return (mycss_entry_t*)myhtml_calloc(1, sizeof(mycss_entry_t));
}

mycss_status_t mycss_entry_init(mycss_t* mycss, mycss_entry_t* entry)
{
    entry->mycss               = mycss;
    entry->parser              = NULL;
    entry->parser_switch       = NULL;
    entry->parser_original     = NULL;
    entry->parser_ending_token = MyCSS_TOKEN_TYPE_UNDEF;
    
    // Other init
    entry->mchar = mchar_async_create(128, (4096 * 5));
    entry->mchar_node_id = mchar_async_node_add(entry->mchar);
    entry->mchar_value_node_id = mchar_async_node_add(entry->mchar);
    
    entry->parser_list = mycss_entry_parser_list_create_and_init(128);
    
    if(entry->parser_list == NULL)
        return MyCSS_STATUS_ERROR_PARSER_LIST_CREATE;
    
    /* String Entries */
    entry->mcobject_string_entries = mcobject_create();
    if(entry->mcobject_string_entries == NULL)
        return MyCSS_STATUS_ERROR_STRING_CREATE;
    
    myhtml_status_t myhtml_status = mcobject_init(entry->mcobject_string_entries, 256, sizeof(myhtml_string_t));
    if(myhtml_status)
        return MyCSS_STATUS_ERROR_STRING_INIT;
    
    /* Selectors */
    entry->selectors = mycss_selectors_create();
    if(entry->selectors == NULL)
        return MyCSS_STATUS_ERROR_SELECTORS_CREATE;
    
    mycss_status_t status = mycss_selectors_init(entry, entry->selectors);
    if(status != MyCSS_STATUS_OK)
        return status;
    
    /* Namespace */
    entry->ns = mycss_namespace_create();
    if(entry->ns == NULL)
        return MyCSS_STATUS_ERROR_NAMESPACE_CREATE;
    
    status = mycss_namespace_init(entry, entry->ns);
    if(status != MyCSS_STATUS_OK)
        return status;
    
    /* An+B */
    entry->anb = mycss_an_plus_b_create();
    if(entry->ns == NULL)
        return MyCSS_STATUS_ERROR_AN_PLUS_B_CREATE;
    
    status = mycss_an_plus_b_init(entry, entry->anb);
    if(status != MyCSS_STATUS_OK)
        return status;
    
    /* Media */
    entry->media = mycss_media_create();
    if(entry->media == NULL)
        return MyCSS_STATUS_ERROR_MEDIA_CREATE;
    
    status = mycss_media_init(entry, entry->media);
    if(status != MyCSS_STATUS_OK)
        return status;
    
    /* Declaration */
    entry->declaration = mycss_declaration_create();
    if(entry->declaration == NULL)
        return MyCSS_STATUS_ERROR_DECLARATION_CREATE;
    
    status = mycss_declaration_init(entry, entry->declaration);
    if(status != MyCSS_STATUS_OK)
        return status;
    
    /* Incoming Buffer */
    entry->mcobject_incoming_buffer = mcobject_create();
    if(entry->mcobject_incoming_buffer == NULL)
        return MyCSS_STATUS_ERROR_ENTRY_INCOMING_BUFFER_CREATE;
    
    myhtml_status = mcobject_init(entry->mcobject_incoming_buffer, 256, sizeof(myhtml_incoming_buffer_t));
    if(myhtml_status)
        return MyCSS_STATUS_ERROR_ENTRY_INCOMING_BUFFER_INIT;
    
    /* callbacks */
    entry->token_ready_callback = mycss_parser_token_ready_callback_function;
    
    return MyCSS_STATUS_OK;
}

mycss_status_t mycss_entry_clean(mycss_entry_t* entry)
{
    mcobject_clean(entry->mcobject_incoming_buffer);
    mycss_entry_parser_list_clean(entry->parser_list);
    
    /* CSS Modules */
    mycss_selectors_clean(entry->selectors);
    mycss_namespace_clean(entry->ns);
    mycss_declaration_clean(entry->declaration);
    
    entry->parser               = NULL;
    entry->parser_switch        = NULL;
    entry->parser_original      = NULL;
    entry->parser_ending_token  = MyCSS_TOKEN_TYPE_UNDEF;
    entry->state                = MyCSS_TOKENIZER_STATE_DATA;
    entry->state_back           = MyCSS_TOKENIZER_STATE_DATA;
    entry->first_buffer         = NULL;
    entry->current_buffer       = NULL;
    entry->token_counter        = 0;
    entry->help_counter         = 0;
    entry->type                 = MyCSS_ENTRY_TYPE_CLEAN;
    
    return MyCSS_STATUS_OK;
}

mycss_status_t mycss_entry_clean_all(mycss_entry_t* entry)
{
    mcobject_clean(entry->mcobject_incoming_buffer);
    mchar_async_node_clean(entry->mchar, entry->mchar_node_id);
    mchar_async_node_clean(entry->mchar, entry->mchar_value_node_id);
    
    mycss_entry_parser_list_clean(entry->parser_list);
    
    mcobject_clean(entry->mcobject_string_entries);
    
    /* CSS Modules */
    mycss_selectors_clean_all(entry->selectors);
    mycss_namespace_clean_all(entry->ns);
    mycss_an_plus_b_clean_all(entry->anb);
    mycss_media_clean_all(entry->media);
    mycss_declaration_clean_all(entry->declaration);
    
    entry->parser               = NULL;
    entry->parser_switch        = NULL;
    entry->parser_original      = NULL;
    entry->parser_ending_token  = MyCSS_TOKEN_TYPE_UNDEF;
    entry->state                = MyCSS_TOKENIZER_STATE_DATA;
    entry->state_back           = MyCSS_TOKENIZER_STATE_DATA;
    entry->first_buffer         = NULL;
    entry->current_buffer       = NULL;
    entry->token_counter        = 0;
    entry->help_counter         = 0;
    entry->type                 = MyCSS_ENTRY_TYPE_CLEAN;
    
    return MyCSS_STATUS_OK;
}

mycss_entry_t * mycss_entry_destroy(mycss_entry_t* entry, bool self_destroy)
{
    if(entry == NULL)
        return NULL;
    
    entry->mchar = mchar_async_destroy(entry->mchar, 1);
    entry->mcobject_string_entries = mcobject_destroy(entry->mcobject_string_entries, true);
    entry->parser_list = mycss_entry_parser_list_destroy(entry->parser_list, true);
    
    /* CSS Modules */
    entry->selectors   = mycss_selectors_destroy(entry->selectors, true);
    entry->ns          = mycss_namespace_destroy(entry->ns, true);
    entry->anb         = mycss_an_plus_b_destroy(entry->anb, true);
    entry->media       = mycss_media_destroy(entry->media, true);
    entry->declaration = mycss_declaration_destroy(entry->declaration, true);
    
    entry->mcobject_incoming_buffer = mcobject_destroy(entry->mcobject_incoming_buffer, true);
    
    if(entry->token) {
        myhtml_free(entry->token);
        entry->token = NULL;
    }
    
    if(self_destroy) {
        myhtml_free(entry);
        return NULL;
    }
    
    return entry;
}

void mycss_entry_end(mycss_entry_t* entry)
{
    /* need some code */
}

/* api */
mycss_selectors_t * mycss_entry_selectors(mycss_entry_t* entry)
{
    return entry->selectors;
}

myhtml_string_t * mycss_entry_string_create_and_init(mycss_entry_t* entry, size_t string_size)
{
    myhtml_string_t *str = mcobject_malloc(entry->mcobject_string_entries, NULL);
    
    if(str == NULL)
        return NULL;
    
    myhtml_string_init(entry->mchar, entry->mchar_node_id, str, (string_size + 1));
    
    return str;
}

mycss_token_ready_callback_f mycss_entry_token_ready_callback(mycss_entry_t* entry, mycss_token_ready_callback_f callback_f)
{
    if(callback_f)
        entry->token_ready_callback = callback_f;
    
    return entry->token_ready_callback;
}

size_t mycss_entry_token_count(mycss_entry_t* entry)
{
    return entry->token_counter;
}

myhtml_incoming_buffer_t * mycss_entry_incoming_buffer_current(mycss_entry_t* entry)
{
    return entry->current_buffer;
}

myhtml_incoming_buffer_t * mycss_entry_incoming_buffer_first(mycss_entry_t* entry)
{
    return entry->first_buffer;
}

mycss_stylesheet_t * mycss_entry_stylesheet(mycss_entry_t* entry)
{
    return entry->stylesheet;
}

mycss_selectors_list_t * mycss_entry_current_selectors_list(mycss_entry_t* entry)
{
    return entry->selectors->list_last;
}

void mycss_entry_parser_set(mycss_entry_t* entry, mycss_parser_token_f parser)
{
    entry->parser = parser;
}

void mycss_entry_parser_switch_set(mycss_entry_t* entry, mycss_parser_token_f parser_switch)
{
    entry->parser_switch = parser_switch;
}

void mycss_entry_parser_original_set(mycss_entry_t* entry, mycss_parser_token_f parser_original)
{
    entry->parser_original = parser_original;
}

/* parser list */
mycss_entry_parser_list_t * mycss_entry_parser_list_create_and_init(size_t size)
{
    mycss_entry_parser_list_t* parser_list = myhtml_malloc(sizeof(mycss_entry_parser_list_t));
    
    if(parser_list == NULL)
        return NULL;
    
    parser_list->length = 0;
    parser_list->size   = size;
    parser_list->list   = myhtml_malloc(parser_list->size * sizeof(mycss_entry_parser_list_entry_t));
    
    if(parser_list->list == NULL) {
        myhtml_free(parser_list);
        return NULL;
    }
    
    return parser_list;
}

void mycss_entry_parser_list_clean(mycss_entry_parser_list_t* parser_list)
{
    parser_list->length = 0;
}

mycss_entry_parser_list_t * mycss_entry_parser_list_destroy(mycss_entry_parser_list_t* parser_list, bool self_destroy)
{
    if(parser_list == NULL)
        return NULL;
    
    if(parser_list->list) {
        myhtml_free(parser_list->list);
        parser_list->list = NULL;
    }
    
    if(self_destroy) {
        myhtml_free(parser_list);
        return NULL;
    }
    
    return parser_list;
}

mycss_status_t mycss_entry_parser_list_push(mycss_entry_t* entry, mycss_parser_token_f parser_func,
                                            mycss_parser_token_f parser_switch, mycss_token_type_t ending_token,
                                            bool is_local)
{
    mycss_entry_parser_list_t *parser_list = entry->parser_list;
    
    if(parser_list->length >= parser_list->size) {
        size_t new_size = parser_list->length + 1024;
        
        mycss_entry_parser_list_entry_t *new_list = myhtml_realloc(parser_list->list, new_size * sizeof(mycss_entry_parser_list_entry_t));
        
        if(new_list) {
            parser_list->size = new_size;
            parser_list->list = new_list;
        }
        else
            return MyCSS_STATUS_ERROR_MEMORY_ALLOCATION;
    }
    
    mycss_entry_parser_list_entry_t *parser_list_entry = &parser_list->list[ parser_list->length ];
    
    parser_list_entry->parser        = parser_func;
    parser_list_entry->parser_switch = parser_switch;
    parser_list_entry->ending_token  = ending_token;
    parser_list_entry->is_local      = is_local;
    parser_list->length++;
    
    return MyCSS_STATUS_OK;
}

void mycss_entry_parser_list_pop(mycss_entry_t* entry)
{
    if(entry->parser_list->length == 0)
        return;
    
    mycss_entry_parser_list_t *parser_list = entry->parser_list;
    parser_list->length--;
    
    mycss_entry_parser_list_entry_t *list_entery = &parser_list->list[ parser_list->length ];
    
    if(entry->parser_ending_token != list_entery->ending_token)
        entry->parser_ending_token = list_entery->ending_token;
    
    entry->parser = list_entery->parser;
    entry->parser_switch = list_entery->parser_switch;
}

size_t mycss_entry_parser_list_length(mycss_entry_t* entry)
{
    return entry->parser_list->length;
}

mycss_token_type_t mycss_entry_parser_list_current_ending_token_type(mycss_entry_t* entry)
{
    return entry->parser_list->list[ (entry->parser_list->length - 1) ].ending_token;
}

mycss_parser_token_f mycss_entry_parser_list_current_parser(mycss_entry_t* entry)
{
    return entry->parser_list->list[ (entry->parser_list->length - 1) ].parser;
}

mycss_parser_token_f mycss_entry_parser_list_current_parser_switch(mycss_entry_t* entry)
{
    return entry->parser_list->list[ (entry->parser_list->length - 1) ].parser_switch;
}

bool mycss_entry_parser_list_current_is_local(mycss_entry_t* entry)
{
    return entry->parser_list->list[ (entry->parser_list->length - 1) ].is_local;
}
// #include <mycss/myosi.h>
// #include <mycss/tokenizer.h>
// #include <myhtml/myhtml.h>
// #include <myhtml/utils/mcobject_async.h>
/*
 Copyright (C) 2016 Alexander Borisov
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 
 Author: lex.borisov@gmail.com (Alexander Borisov)
*/

#ifndef MyHTML_MyCSS_MYCSS_H
#define MyHTML_MyCSS_MYCSS_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MyCSS_DEBUG
    #define MyCSS_DEBUG_MESSAGE(format, ...) \
        myhtml_print(stderr, "DEBUG: "format"\n", ##__VA_ARGS__)
#else
    #define MyCSS_DEBUG_MESSAGE(format, ...)
#endif

#ifdef DEBUG_MODE
#define MyHTML_DEBUG_ERROR(format, ...)      \
myhtml_print(stderr, "DEBUG ERROR: "format"\n", ##__VA_ARGS__)
#else
#define MyHTML_DEBUG_ERROR(format, ...)
#endif

struct mycss {
    mycss_tokenizer_state_f* parse_state_func;
};

mycss_t * mycss_create(void);
mycss_status_t mycss_init(mycss_t* mycss);
mycss_t * mycss_destroy(mycss_t* mycss, bool self_destroy);

mycss_status_t mycss_parse(mycss_entry_t* entry, myhtml_encoding_t encoding, const char* css, size_t css_size);
mycss_status_t mycss_parse_chunk(mycss_entry_t* entry, const char* css, size_t css_size);
mycss_status_t mycss_parse_chunk_end(mycss_entry_t* entry);

size_t mycss_token_begin(mycss_token_t* token);
size_t mycss_token_length(mycss_token_t* token);
size_t mycss_token_position(mycss_token_t* token, size_t *return_length);
mycss_token_type_t mycss_token_type(mycss_token_t* token);
const char * mycss_token_name_by_type(mycss_token_type_t type);
size_t mycss_token_data_to_string(mycss_entry_t* entry, mycss_token_t* token, myhtml_string_t* str, bool init_string, bool case_insensitive);

myhtml_incoming_buffer_t * mycss_token_buffer_first(mycss_entry_t* entry, mycss_token_t* token);

// encoding
void mycss_encoding_set(mycss_entry_t* entry, myhtml_encoding_t encoding);
myhtml_encoding_t mycss_encoding_get(mycss_entry_t* entry);
myhtml_encoding_t mycss_encoding_check_charset_rule(const char* css, size_t size);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

/*
 Copyright (C) 2016 Alexander Borisov
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 
 Author: lex.borisov@gmail.com (Alexander Borisov)
*/

#ifndef MyHTML_MyCSS_TOKENIZER_RESOURCE_H
#define MyHTML_MyCSS_TOKENIZER_RESOURCE_H

#ifdef __cplusplus
extern "C" {
#endif

//
// generated by Perl script utils/mycss_first_char.pl
static const mycss_tokenizer_state_t mycss_begin_chars_state_map[] = {
    MyCSS_TOKENIZER_STATE_DELIM_SINGLE_CODE_POINT, //
    MyCSS_TOKENIZER_STATE_DELIM_SINGLE_CODE_POINT, //
    MyCSS_TOKENIZER_STATE_DELIM_SINGLE_CODE_POINT, //
    MyCSS_TOKENIZER_STATE_DELIM_SINGLE_CODE_POINT, //
    MyCSS_TOKENIZER_STATE_DELIM_SINGLE_CODE_POINT, //
    MyCSS_TOKENIZER_STATE_DELIM_SINGLE_CODE_POINT, //
    MyCSS_TOKENIZER_STATE_DELIM_SINGLE_CODE_POINT, //
    MyCSS_TOKENIZER_STATE_DELIM_SINGLE_CODE_POINT, //
    MyCSS_TOKENIZER_STATE_DELIM_SINGLE_CODE_POINT, //
    MyCSS_TOKENIZER_STATE_WHITESPACE,              // whitespace
    MyCSS_TOKENIZER_STATE_WHITESPACE,              // whitespace
    MyCSS_TOKENIZER_STATE_DELIM_SINGLE_CODE_POINT, //
    MyCSS_TOKENIZER_STATE_WHITESPACE,              // whitespace
    MyCSS_TOKENIZER_STATE_WHITESPACE,              // whitespace
    MyCSS_TOKENIZER_STATE_DELIM_SINGLE_CODE_POINT, //
    MyCSS_TOKENIZER_STATE_DELIM_SINGLE_CODE_POINT, //
    MyCSS_TOKENIZER_STATE_DELIM_SINGLE_CODE_POINT, //
    MyCSS_TOKENIZER_STATE_DELIM_SINGLE_CODE_POINT, //
    MyCSS_TOKENIZER_STATE_DELIM_SINGLE_CODE_POINT, //
    MyCSS_TOKENIZER_STATE_DELIM_SINGLE_CODE_POINT, //
    MyCSS_TOKENIZER_STATE_DELIM_SINGLE_CODE_POINT, //
    MyCSS_TOKENIZER_STATE_DELIM_SINGLE_CODE_POINT, //
    MyCSS_TOKENIZER_STATE_DELIM_SINGLE_CODE_POINT, //
    MyCSS_TOKENIZER_STATE_DELIM_SINGLE_CODE_POINT, //
    MyCSS_TOKENIZER_STATE_DELIM_SINGLE_CODE_POINT, //
    MyCSS_TOKENIZER_STATE_DELIM_SINGLE_CODE_POINT, //
    MyCSS_TOKENIZER_STATE_DELIM_SINGLE_CODE_POINT, //
    MyCSS_TOKENIZER_STATE_DELIM_SINGLE_CODE_POINT, //
    MyCSS_TOKENIZER_STATE_DELIM_SINGLE_CODE_POINT, //
    MyCSS_TOKENIZER_STATE_DELIM_SINGLE_CODE_POINT, //
    MyCSS_TOKENIZER_STATE_DELIM_SINGLE_CODE_POINT, //
    MyCSS_TOKENIZER_STATE_DELIM_SINGLE_CODE_POINT, //
    MyCSS_TOKENIZER_STATE_WHITESPACE,              // whitespace
    MyCSS_TOKENIZER_STATE_DELIM_SINGLE_CODE_POINT, //
    MyCSS_TOKENIZER_STATE_QUOTATION_MARK,          // U+0022 QUOTATION MARK (")
    MyCSS_TOKENIZER_STATE_NUMBER_SIGN,             // U+0023 NUMBER SIGN (#)
    MyCSS_TOKENIZER_STATE_DOLLAR_SIGN,             // U+0024 DOLLAR SIGN ($)
    MyCSS_TOKENIZER_STATE_DELIM_SINGLE_CODE_POINT, //
    MyCSS_TOKENIZER_STATE_DELIM_SINGLE_CODE_POINT, //
    MyCSS_TOKENIZER_STATE_APOSTROPHE,              // U+0027 APOSTROPHE (')
    MyCSS_TOKENIZER_STATE_LEFT_PARENTHESIS,        // U+0028 LEFT PARENTHESIS (()
    MyCSS_TOKENIZER_STATE_RIGHT_PARENTHESIS,       // U+0029 RIGHT PARENTHESIS ())
    MyCSS_TOKENIZER_STATE_ASTERISK,                // U+002A ASTERISK (*)
    MyCSS_TOKENIZER_STATE_PLUS_SIGN,               // U+002B PLUS SIGN (+)
    MyCSS_TOKENIZER_STATE_COMMA,                   // U+002C COMMA (,)
    MyCSS_TOKENIZER_STATE_HYPHEN_MINUS,            // U+002D HYPHEN-MINUS (-)
    MyCSS_TOKENIZER_STATE_FULL_STOP,               // U+002E FULL STOP (.)
    MyCSS_TOKENIZER_STATE_SOLIDUS,                 // U+002F SOLIDUS (/)
    MyCSS_TOKENIZER_STATE_DIGIT,                   // digit
    MyCSS_TOKENIZER_STATE_DIGIT,                   // digit
    MyCSS_TOKENIZER_STATE_DIGIT,                   // digit
    MyCSS_TOKENIZER_STATE_DIGIT,                   // digit
    MyCSS_TOKENIZER_STATE_DIGIT,                   // digit
    MyCSS_TOKENIZER_STATE_DIGIT,                   // digit
    MyCSS_TOKENIZER_STATE_DIGIT,                   // digit
    MyCSS_TOKENIZER_STATE_DIGIT,                   // digit
    MyCSS_TOKENIZER_STATE_DIGIT,                   // digit
    MyCSS_TOKENIZER_STATE_DIGIT,                   // digit
    MyCSS_TOKENIZER_STATE_COLON,                   // U+003A COLON (:)
    MyCSS_TOKENIZER_STATE_SEMICOLON,               // U+003B SEMICOLON (;)
    MyCSS_TOKENIZER_STATE_LESS_THAN_SIGN,          // U+003C LESS-THAN SIGN (<)
    MyCSS_TOKENIZER_STATE_DELIM_SINGLE_CODE_POINT, //
    MyCSS_TOKENIZER_STATE_DELIM_SINGLE_CODE_POINT, //
    MyCSS_TOKENIZER_STATE_DELIM_SINGLE_CODE_POINT, //
    MyCSS_TOKENIZER_STATE_COMMERCIAL_AT,           // U+0040 COMMERCIAL AT (@)
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_LETTER_U,                // name-start code point and LATIN CAPITAL AND SMALL LETTER U
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_LEFT_SQUARE_BRACKET,     // U+005B LEFT SQUARE BRACKET ([)
    MyCSS_TOKENIZER_STATE_REVERSE_SOLIDUS,         // U+005C REVERSE SOLIDUS ()
    MyCSS_TOKENIZER_STATE_RIGHT_SQUARE_BRACKET,    // U+005D RIGHT SQUARE BRACKET (])
    MyCSS_TOKENIZER_STATE_CIRCUMFLEX_ACCENT,       // U+005E CIRCUMFLEX ACCENT (^)
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_DELIM_SINGLE_CODE_POINT, //
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_LETTER_U,                // name-start code point and LATIN CAPITAL AND SMALL LETTER U
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_LEFT_CURLY_BRACKET,      // U+007B LEFT CURLY BRACKET ({)
    MyCSS_TOKENIZER_STATE_VERTICAL_LINE,           // U+007C VERTICAL LINE (|)
    MyCSS_TOKENIZER_STATE_RIGHT_CURLY_BRACKET,     // U+007D RIGHT CURLY BRACKET (})
    MyCSS_TOKENIZER_STATE_TILDE,                   // U+007E TILDE (~)
    MyCSS_TOKENIZER_STATE_DELIM_SINGLE_CODE_POINT, //
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT,   // name-start code point
    MyCSS_TOKENIZER_STATE_NAME_START_CODE_POINT    // name-start code point
};

static const unsigned char mycss_chars_name_code_point_map[] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0x2d, 0xff, 0xff, 0x30, 0x31,
    0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0x41, 0x42, 0x43, 0x44, 0x45,
    0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
    0x5a, 0xff, 0xff, 0xff, 0xff, 0x5f, 0xff, 0x61, 0x62, 0x63,
    0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d,
    0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
    0x78, 0x79, 0x7a, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x81,
    0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b,
    0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95,
    0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
    0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9,
    0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3,
    0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd,
    0xbe, 0xbf, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
    0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1,
    0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb,
    0xdc, 0xdd, 0xde, 0xdf, 0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5,
    0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9,
    0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};

static const char mycss_token_type_description[][32] =
{
    "UNDEF",
    "WHITESPACE",
    "IDENT",
    "FUNCTION",
    "AT_KEYWORD",
    "HASH",
    "STRING",
    "URL",
    "NUMBER",
    "DIMENSION",
    "PERCENTAGE",
    "INCLUDE_MATCH",
    "DASH_MATCH",
    "PREFIX_MATCH",
    "SUFFIX_MATCH",
    "SUBSTRING_MATCH",
    "COLUMN",
    "CDO",
    "CDC",
    "BAD_STRING",
    "DELIM",
    "LEFT_PARENTHESIS",
    "RIGHT_PARENTHESIS",
    "COMMA",
    "COLON",
    "SEMICOLON",
    "LEFT_SQUARE_BRACKET",
    "RIGHT_SQUARE_BRACKET",
    "LEFT_CURLY_BRACKET",
    "RIGHT_CURLY_BRACKET",
    "BAD_URL",
    "COMMENT",
    "UNICODE_RANGE",
    "END_OF_FILE",
    ""
};

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MyHTML_MyCSS_TOKENIZER_RESOURCE_H */

/*
 Copyright (C) 2016 Alexander Borisov
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 
 Author: lex.borisov@gmail.com (Alexander Borisov)
*/

mycss_t * mycss_create(void)
{
    return (mycss_t*)myhtml_calloc(1, sizeof(mycss_t));
}

mycss_status_t mycss_init(mycss_t* mycss)
{
    mycss_status_t status = mycss_tokenizer_state_init(mycss);
    
    if(status != MyCSS_STATUS_OK)
        return status;
    
    return MyCSS_STATUS_OK;
}

mycss_t * mycss_destroy(mycss_t* mycss, bool self_destroy)
{
    if(mycss == NULL)
        return NULL;
    
    mycss_tokenizer_state_destroy(mycss);
    
    if(self_destroy) {
        myhtml_free(mycss);
        return NULL;
    }
    
    return mycss;
}

mycss_status_t mycss_parse(mycss_entry_t* entry, myhtml_encoding_t encoding, const char* css, size_t css_size)
{
    mycss_entry_clean(entry);
    
    /* set parser */
    entry->parser = mycss_parser_token;
    
    /* stylesheet */
    entry->stylesheet = mycss_stylesheet_create();
    mycss_stylesheet_init(entry->stylesheet, entry);
    
    /* and parse css */
    mycss_encoding_set(entry, encoding);
    
    mycss_status_t status = mycss_tokenizer_chunk(entry, css, css_size);
    if(status != MyCSS_STATUS_OK)
        return status;
    
    status = mycss_tokenizer_end(entry);
    
    return status;
}

mycss_status_t mycss_parse_chunk(mycss_entry_t* entry, const char* css, size_t css_size)
{
    if(entry->type & MyCSS_ENTRY_TYPE_END) {
        mycss_entry_clean_all(entry);
    }
    
    /* create and init new Result */
    if(entry->stylesheet == NULL) {
        entry->stylesheet = mycss_stylesheet_create();
        mycss_stylesheet_init(entry->stylesheet, entry);
    }
    
    return mycss_tokenizer_chunk(entry, css, css_size);
}

mycss_status_t mycss_parse_chunk_end(mycss_entry_t* entry)
{
    mycss_status_t status = mycss_tokenizer_end(entry);
    mycss_entry_end(entry);
    
    return status;
}

// token
size_t mycss_token_position(mycss_token_t* token, size_t *return_length)
{
    if(return_length)
        *return_length = token->length;
    
    return token->begin;
}

size_t mycss_token_begin(mycss_token_t* token)
{
    return token->begin;
}

size_t mycss_token_length(mycss_token_t* token)
{
    return token->length;
}

mycss_token_type_t mycss_token_type(mycss_token_t* token)
{
    return token->type;
}

const char * mycss_token_name_by_type(mycss_token_type_t type)
{
    if(type >= MyCSS_TOKEN_TYPE_LAST_ENTRY)
        return mycss_token_type_description[MyCSS_TOKEN_TYPE_LAST_ENTRY];
    
    return mycss_token_type_description[type];
}

size_t mycss_token_data_to_string(mycss_entry_t* entry, mycss_token_t* token, myhtml_string_t* str, bool init_string, bool case_insensitive)
{
    if(init_string)
        myhtml_string_init(entry->mchar, entry->mchar_node_id, str, (token->length + 4));
    
    mycss_string_res_t out_res;
    mycss_string_res_clean(&out_res);
    
    out_res.encoding = entry->encoding;
    out_res.case_insensitive = case_insensitive;
    
    myhtml_incoming_buffer_t *buffer = myhtml_incoming_buffer_find_by_position(entry->current_buffer, token->begin);
    size_t relative_begin = token->begin - buffer->offset;
    
    // if token data length in one buffer then print them all at once
    if((relative_begin + token->length) <= buffer->size) {
        mycss_string_data_process(str, &buffer->data[relative_begin], 0, token->length, &out_res);
        mycss_string_data_process_end(str, &out_res);
        
        return token->length;
    }
    
    // if the data are spread across multiple buffers that join them
    size_t length = token->length;
    while(buffer) {
        if((relative_begin + length) > buffer->size)
        {
            size_t relative_end = (buffer->size - relative_begin);
            length -= relative_end;
            
            mycss_string_data_process(str, buffer->data, relative_begin, relative_end, &out_res);
            
            relative_begin = 0;
            buffer         = buffer->next;
        }
        else {
            mycss_string_data_process(str, &buffer->data[relative_begin], 0, length, &out_res);
            break;
        }
    }
    
    mycss_string_data_process_end(str, &out_res);
    
    return token->length;
}

myhtml_incoming_buffer_t * mycss_token_buffer_first(mycss_entry_t* entry, mycss_token_t* token)
{
    return myhtml_incoming_buffer_find_by_position(entry->current_buffer, token->begin);
}

// encoding

void mycss_encoding_set(mycss_entry_t* entry, myhtml_encoding_t encoding)
{
    entry->encoding = encoding;
}

myhtml_encoding_t mycss_encoding_get(mycss_entry_t* entry)
{
    return entry->encoding;
}

myhtml_encoding_t mycss_encoding_check_charset_rule(const char* css, size_t size)
{
    if(size < 15)
        return MyHTML_ENCODING_UTF_8;
    
    if(strncmp("@charset \"", css, 10))
        return MyHTML_ENCODING_UTF_8;
    
    size_t begin = 10;
    size_t length = begin;
    
    while(length < size) {
        if(css[length] == '"')
        {
            ++length;
            
            if(length >= size || css[length] != ';')
                return MyHTML_ENCODING_UTF_8;
            
            size_t name_len = (length - begin) - 1;
            
            myhtml_encoding_t encoding;
            if(myhtml_encoding_by_name(&css[begin], name_len, &encoding))
                return encoding;
            
            break;
        }
        
        ++length;
    }
    
    return MyHTML_ENCODING_UTF_8;
}

/*
 Copyright (C) 2016 Alexander Borisov
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 
 Author: lex.borisov@gmail.com (Alexander Borisov)
*/

mycss_t * mycss_create(void)
{
    return (mycss_t*)myhtml_calloc(1, sizeof(mycss_t));
}

mycss_status_t mycss_init(mycss_t* mycss)
{
    mycss_status_t status = mycss_tokenizer_state_init(mycss);
    
    if(status != MyCSS_STATUS_OK)
        return status;
    
    return MyCSS_STATUS_OK;
}

mycss_t * mycss_destroy(mycss_t* mycss, bool self_destroy)
{
    if(mycss == NULL)
        return NULL;
    
    mycss_tokenizer_state_destroy(mycss);
    
    if(self_destroy) {
        myhtml_free(mycss);
        return NULL;
    }
    
    return mycss;
}

mycss_status_t mycss_parse(mycss_entry_t* entry, myhtml_encoding_t encoding, const char* css, size_t css_size)
{
    mycss_entry_clean(entry);
    
    /* set parser */
    entry->parser = mycss_parser_token;
    
    /* stylesheet */
    entry->stylesheet = mycss_stylesheet_create();
    mycss_stylesheet_init(entry->stylesheet, entry);
    
    /* and parse css */
    mycss_encoding_set(entry, encoding);
    
    mycss_status_t status = mycss_tokenizer_chunk(entry, css, css_size);
    if(status != MyCSS_STATUS_OK)
        return status;
    
    status = mycss_tokenizer_end(entry);
    
    return status;
}

mycss_status_t mycss_parse_chunk(mycss_entry_t* entry, const char* css, size_t css_size)
{
    if(entry->type & MyCSS_ENTRY_TYPE_END) {
        mycss_entry_clean_all(entry);
    }
    
    /* create and init new Result */
    if(entry->stylesheet == NULL) {
        entry->stylesheet = mycss_stylesheet_create();
        mycss_stylesheet_init(entry->stylesheet, entry);
    }
    
    return mycss_tokenizer_chunk(entry, css, css_size);
}

mycss_status_t mycss_parse_chunk_end(mycss_entry_t* entry)
{
    mycss_status_t status = mycss_tokenizer_end(entry);
    mycss_entry_end(entry);
    
    return status;
}

// token
size_t mycss_token_position(mycss_token_t* token, size_t *return_length)
{
    if(return_length)
        *return_length = token->length;
    
    return token->begin;
}

size_t mycss_token_begin(mycss_token_t* token)
{
    return token->begin;
}

size_t mycss_token_length(mycss_token_t* token)
{
    return token->length;
}

mycss_token_type_t mycss_token_type(mycss_token_t* token)
{
    return token->type;
}

const char * mycss_token_name_by_type(mycss_token_type_t type)
{
    if(type >= MyCSS_TOKEN_TYPE_LAST_ENTRY)
        return mycss_token_type_description[MyCSS_TOKEN_TYPE_LAST_ENTRY];
    
    return mycss_token_type_description[type];
}

size_t mycss_token_data_to_string(mycss_entry_t* entry, mycss_token_t* token, myhtml_string_t* str, bool init_string, bool case_insensitive)
{
    if(init_string)
        myhtml_string_init(entry->mchar, entry->mchar_node_id, str, (token->length + 4));
    
    mycss_string_res_t out_res;
    mycss_string_res_clean(&out_res);
    
    out_res.encoding = entry->encoding;
    out_res.case_insensitive = case_insensitive;
    
    myhtml_incoming_buffer_t *buffer = myhtml_incoming_buffer_find_by_position(entry->current_buffer, token->begin);
    size_t relative_begin = token->begin - buffer->offset;
    
    // if token data length in one buffer then print them all at once
    if((relative_begin + token->length) <= buffer->size) {
        mycss_string_data_process(str, &buffer->data[relative_begin], 0, token->length, &out_res);
        mycss_string_data_process_end(str, &out_res);
        
        return token->length;
    }
    
    // if the data are spread across multiple buffers that join them
    size_t length = token->length;
    while(buffer) {
        if((relative_begin + length) > buffer->size)
        {
            size_t relative_end = (buffer->size - relative_begin);
            length -= relative_end;
            
            mycss_string_data_process(str, buffer->data, relative_begin, relative_end, &out_res);
            
            relative_begin = 0;
            buffer         = buffer->next;
        }
        else {
            mycss_string_data_process(str, &buffer->data[relative_begin], 0, length, &out_res);
            break;
        }
    }
    
    mycss_string_data_process_end(str, &out_res);
    
    return token->length;
}

myhtml_incoming_buffer_t * mycss_token_buffer_first(mycss_entry_t* entry, mycss_token_t* token)
{
    return myhtml_incoming_buffer_find_by_position(entry->current_buffer, token->begin);
}

// encoding

void mycss_encoding_set(mycss_entry_t* entry, myhtml_encoding_t encoding)
{
    entry->encoding = encoding;
}

myhtml_encoding_t mycss_encoding_get(mycss_entry_t* entry)
{
    return entry->encoding;
}

myhtml_encoding_t mycss_encoding_check_charset_rule(const char* css, size_t size)
{
    if(size < 15)
        return MyHTML_ENCODING_UTF_8;
    
    if(strncmp("@charset \"", css, 10))
        return MyHTML_ENCODING_UTF_8;
    
    size_t begin = 10;
    size_t length = begin;
    
    while(length < size) {
        if(css[length] == '"')
        {
            ++length;
            
            if(length >= size || css[length] != ';')
                return MyHTML_ENCODING_UTF_8;
            
            size_t name_len = (length - begin) - 1;
            
            myhtml_encoding_t encoding;
            if(myhtml_encoding_by_name(&css[begin], name_len, &encoding))
                return encoding;
            
            break;
        }
        
        ++length;
    }
    
    return MyHTML_ENCODING_UTF_8;
}
