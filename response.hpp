#ifndef RESPONSE_HPP_
#define RESPONSE_HPP_

#include "parsing.hpp"
#include "request.hpp"

#define DEFAULT_MIME_TYPE "application/octet-stream"
#define HTTP_V "HTTP/1.1 "
#define SERVER "Server: Sarii/v1.0\r\n"

class response {
	// std::map<std::string, std::map<std::string, std::string> >::const_iterator it;
	std::map<std::string, std::string>::const_iterator iter;

	const data_serv 	*server_data;
	data_header			*header_data;
	char				buffer[BUFFER_SIZE];

	// Content				response_content;
	std::ifstream		in_file;
	// std::string			new_uri;
	// std::string			query;
	std::string			target;
	std::string			requested_resource;
	// std::string			methods;

	// std::string			redirect_path;
	int					status_code;
	bool				is_header;
	bool				is_body;
	// bool				is_redirect;
	bool				is_read;

	std::string			header;
	

public:
	bool	eof;
	response( const data_serv *,  data_header* );

	void	response_handler( int, std::string& );
	/* check for location and redirections and allowed methods */
	void	check_location_block();
	
	/*check if requested resorce in such location */
	int		find_required_location(  );
	void	parse_the_uri();
	int		update_the_uri();
	/* check if there is any redirection */
	int		redirection();
	/* check for allowed methods */
	int 	allowed_methods();
	
	/* create response header */
	void	create_header();

	void	Error_pages();



	void	GET_handler( int );


	
	int 	upload_file();
	int		requested_resource_is_dir();

	// void	send_response();
	int		serve_the_file();
	const std::string	get_extension( const std::string& );
	std::string	Get_Content_Type();
	std::string	Get_Content_Length();
	std::string time_date();
	std::string	get_start_line();

	/* some helpful functions */
	int		search_inside_location( const std::string );
	// int		path_is_exist( const char * );
	int		is_file_or_directory( const char* );

	/* GET method */
	void	Get_method();

	/* POST method */
	void	Post_method();

	/* Delete method */
	void	Delete_method();
};

#endif