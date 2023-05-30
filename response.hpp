#ifndef RESPONSE_HPP_
#define RESPONSE_HPP_
<<<<<<< HEAD
#include <iostream>

=======

<<<<<<< HEAD
#include "request.hpp"
>>>>>>> 243ad6e7211f47db6f73f9995fdf167efdfa8cce
=======
#include "parsing.hpp"
>>>>>>> 7456fb80c15f57cb9f3e712b9a85ddab7f81f1ff

class response {
	std::string msg;
	public:
<<<<<<< HEAD
<<<<<<< HEAD
		response() {
			
		}
		void create_header()
		{
			msg = "HTTP/1.1 " + std::to_string(req->response_status);
			if (req->response_status == 501){
				msg += " Not implemented\r\nContent-Type: text/html\r\n\r\n";
				msg += "<html><body><h1>501 - Not implemented!</h1></body></html>";
			}
			else if (req->response_status == 400){
				msg + " Bad Request\r\nContent-Type: text/html\r\n\r\n";
				msg + "<html><body><h1>400 - Bad request!</h1></body></html>";
		
			}
			else if (req->response_status == 414){
				msg + " Request-URI Too Long\r\nContent-Type: text/html\r\n\r\n";
				msg + "<html><body><h1>414 - Request-Uri Too large!</h1></body></html>";
			
			}
			else if (req->response_status == 413){
				msg += " Request Entity Too large\r\nContent-Type: text/html\r\n\r\n";
				msg += "<html><body><h1>413 - Request Entity Too large!</h1></body></html>";
		
			}
			else if (req->response_status == 404){
				msg += " Not Found\r\nContent-Type: text/html\r\n\r\n";
				msg += "<html><body><h1>404 - Not Found!</h1></body></html>";
			}
			else if (req->response_status == 301){
				msg += " Moved Permanently\r\nContent-Type: text/html\r\n\r\n";
				msg += "<html><body><h1>301 Moved Permanently!</h1></body></html>";
			}
			else if (req->response_status == 405){

				msg += " Method Not Allowed\r\nContent-Type: text/html\r\n\r\n";
				msg += "<html><body><h1>405 - Method Not Allowed!</h1></body></html>";
			}
			else if (req->response_status == 403){
				msg += " Forbidden\r\nContent-Type: text/html\r\n\r\n";
				msg += "<html><body><h1>403 - Forbidden!</h1></body></html>";
			}
			else if (req->response_status == 200){
				msg += " OK\r\nContent-Type: text/html\r\n\r\n";
			}
			else if (req->response_status == 201)
			{
				msg += " Created\r\nContent-Type: text/html\r\n\r\n";
			}
		}
=======
	request		client_request;
=======
	// request		client_request;
>>>>>>> 7456fb80c15f57cb9f3e712b9a85ddab7f81f1ff
		response() {}
>>>>>>> 243ad6e7211f47db6f73f9995fdf167efdfa8cce
};







#endif