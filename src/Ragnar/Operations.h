#pragma once
namespace Ragnar
{
	namespace Unsafe
	{
		// these constants are used to identify the operation that failed, causing a
		// peer to disconnect
		public enum class operation_t : int
		{
			// this is used when the bittorrent logic
			// determines to disconnect
			op_bittorrent = 0,

			// a call to iocontrol failed
			op_iocontrol,

			// a call to getpeername failed (querying the remote IP of a
			// connection)
			op_getpeername,

			// a call to getname failed (querying the local IP of a
			// connection)
			op_getname,

			// an attempt to allocate a receive buffer failed
			op_alloc_recvbuf,

			// an attempt to allocate a send buffer failed
			op_alloc_sndbuf,

			// writing to a file failed
			op_file_write,

			// reading from a file failed
			op_file_read,

			// a non-read and non-write file operation failed
			op_file,

			// a socket write operation failed
			op_sock_write,

			// a socket read operation failed
			op_sock_read,

			// a call to open(), to create a socket socket failed
			op_sock_open,

			// a call to bind() on a socket failed
			op_sock_bind,

			// an attempt to query the number of bytes available to read from a socket
			// failed
			op_available,

			// a call related to bittorrent protocol encryption failed
			op_encryption,

			// an attempt to connect a socket failed
			op_connect,

			// establishing an SSL connection failed
			op_ssl_handshake,

			// a connection failed to satisfy the bind interface setting
			op_get_interface
		};

	}
}