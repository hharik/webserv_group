#ifndef RESPONSE_HPP_
#define RESPONSE_HPP_

#include "parsing.hpp"
#include "request.hpp"
#include <_types/_intmax_t.h>
#include <fstream>
#include <string>
#include <sys/_types/_pid_t.h>
#include <sys/_types/_size_t.h>

#define DEFAULT_MIME_TYPE "application/octet-stream"
#define HTML "text/html"
#define HTTP_V "HTTP/1.1 "
#define SERVER "Server: Sarii/v1.0\r\n"
#define S200 " OK"
#define S201 " Created"
#define S204 " No Content"
#define S207 " Multi-Status"
#define S301 " Moved Permanently"
#define S302 " Found"
#define S400 " Bad Request"
#define S403 " Forbidden"
#define S404 " Not Found"
#define S405 " Method Not Allowed"
#define S409 " Conflict"
#define S414 " URI Too Long"
#define S415 " Unsupported Media Type"
#define S431 " Request Header Fields Too Large"
#define S500 " Internal Server Error"
#define S501 " Not Implemented"

class response {
	std::map<std::string, std::string>::const_iterator iter;
	std::map<int, std::string>::const_iterator er_it;
	std::map<std::string, std::string> cgi_header;

	const data_serv 	*server_data;
	data_header			*header_data;
	char				buffer[BUFFER_SIZE];

	std::ifstream		requested_file;
	std::string			response_content;
	bool				is_open;
	bool				default_response;
	int					s204;
	int					s403;
	bool				auto_index;
	std::string			auto_index_content;
	std::streampos		cgi_body_pos;
	std::string			cgi_start_line;
	size_t				content_length;
	size_t				sended_bytes;
	/*####################################*/
	int		cgifd[2];
	char	*agv[3];
	std::string cgi_output;
	std::vector<std::string> Env;
	pid_t		pid;
	bool		is_alive;
	// std::ifstream cgi_outputfile;
	/*####################################*/

	std::string			header;
	

public:
	bool	eof;
	response( const data_serv *,  data_header* );
	~response( );
	void	response_handler( int );
	// /* check for location and redirections and allowed methods */
	// void	check_location_block();
	
	// /*check if requested resorce in such location */
	// int		find_required_location(  );
	// void	parse_the_uri();
	// int		update_the_uri();
	// /* check if there is any redirection */
	// int		redirection();
	// /* check for allowed methods */
	// int 	allowed_methods();
	
	/* create response header */
	void	create_header();

	void			Pages();
	void	treat_target_resource( const char * );
	void	Send_the_Body( int );
	void	Send_the_Header( int );
	// void	get_requested_resource();
	int		delete_dir( const char * );
	int		delete_the_file();
	int		file_status();
	void	delete_path( const char *path);

	int		requested_resource_is_dir();

	// void	send_response();
	int		serve_the_file();
	const std::string	get_extension( const std::string& );
	std::string	Get_Content_Type();
	std::string	Get_Content_Length();
	std::string Get_Date();
	std::string	get_start_line();
	std::string	get_Location();
	std::string	Default_Response( const std::string&, const std::string& );
	/* some helpful functions */
	int		search_inside_location( const std::string );
	// int		path_is_exist( const char * );
	bool	Is_End_Of_File();

	/*#############################################*/
	void generateAutoIndex(std::string &directory);
	void	handle_cgi();
	/*#############################################*/
	void	handle_cgi_header();
	void	Parse_cgi_header( std::string cgi_header );
	void	Parse_Line( std::string );
	void	get_res_status( std::string  );
	void	check_for_cookies( std::string& );
	void	Process_Cgi_Header();
	void	Put_Header(const std::string &, const std::string &);
	// void	Send_the_Body_WithCgi( int );
	void	set_eof( bool );
	/* GET method */
	void	Get_method();

	/* POST method */
	void	Post_method();

	/* Delete method */
	void	Delete_method();
};

#endif