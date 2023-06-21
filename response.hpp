/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ajemraou <ajemraou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/20 08:14:08 by ajemraou          #+#    #+#             */
/*   Updated: 2023/06/21 09:24:29 by ajemraou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP_
#define RESPONSE_HPP_

#include "parsing.hpp"
#include "request.hpp"
#include <string>

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
#define S413 " Content Too Large"
#define S414 " URI Too Long"
#define S415 " Unsupported Media Type"
#define S431 " Request Header Fields Too Large"
#define S500 " Internal Server Error"
#define S501 " Not Implemented"
#define S502 " Bad Gateway"
#define S504 " Gateway Timeout"

class response {
	std::map<std::string, std::string>::const_iterator iter;
	std::map<int, std::string>::const_iterator er_it;
	std::map<std::string, std::string> cgi_header;
	std::vector<std::string>			cookies;
	const data_serv 	*server_data;
	data_header			*header_data;
	char				buffer[BUFFER_SIZE];

	std::ifstream		requested_file;
	std::string			response_content;
	std::string			cgi_start_line;
	std::string			cgi_output;
	std::streampos		cgi_body_pos;
	std::string			header;

	bool				is_open;
	bool				default_response;
	bool				auto_index;
	bool				is_alive;

	size_t				content_length;
	size_t				sended_bytes;

	int					s204;
	int					s403;

	/* CGI params */
	std::vector<std::string>	Env;
	long						start_time;
	long						end_time;
	pid_t						pid;
	int							cgifd[2];
	int							*status;
	char						*agv[3];
	char						**env;
	bool						eof;

public:
	response( const data_serv *,  data_header* );
	~response( );

	/* Send the Header and the Body */
	void			Pages();
	void	create_header();
	void	response_handler( int client_fd );
	void	Send_the_Header( int );
	void	Send_the_Body( int );
	/* GET method */
	void	Get_method();
	int		requested_resource_is_dir();
	int		file_status();
	int		serve_the_file();
	void	generateAutoIndex(std::string directory);
	/* CGI */
	void	SetCGI_Env( std::string );
	void	handle_cgi();
	int		handle_CGI_timeOut();
	int		CheckTheCGI_Process();
	/* POST method */
	void	Post_method();
	/* Delete method */
	void	Delete_method();
	int		delete_the_file();
	void	delete_path( const char *path, bool );
	DIR		*open_dir( const char *directory );
	int		delete_dir( const char * );
	/* Create the response header */
	std::string		Default_Response( const std::string&, const std::string& );
	std::string		get_start_line();
	std::string		Get_Content_Type();
	std::string		Get_Content_Length();
	std::string 	Get_Date();
	std::string		get_Location();
	/* Parse CGI Header */
	void	Parse_Line( std::string );
	void	Process_Cgi_Header();
	void	Put_Header(const std::string &, const std::string &);
	void	SetCgiStartLine();
	void	handle_cgi_header();
	void	Parse_cgi_header( std::string cgi_header );
	/* Utils member functions */
	int		search_inside_location( const std::string );
	long	get_time();
	bool	IsEndOfFile();
	void	Set_eof( bool );
	bool	Get_eof(  ) const;
};

#endif
