require 'socket'
require 'timeout'

class Socket
  TIMEOUT_SECONDS = 5 # Prevents infinite waiting

  def initialize(host, port)
    @socket = TCPSocket.open(host, port)
  end

  def send_command(command)
    @socket.puts(command)
  end

  def get_response
    @socket.gets&.chomp || ''
  end

  def get_full_response
    response = []
    begin
      Timeout.timeout(TIMEOUT_SECONDS) do
        while (line = @socket.gets)
          if line.strip.empty?
            break
          end
          response << line.chomp
        end
      end
    rescue Timeout::Error
      puts "⚠️ Warning: Server response timed out after #{TIMEOUT_SECONDS} seconds."
    end
    response.join("\n")
  end

  def get_latest_updates
    return '' unless IO.select([@socket], nil, nil, 0)

    response = []
    loop do
      begin
        line = @socket.read_nonblock(4096)  # Read available data without blocking
        response << line.chomp unless line.empty?
      rescue IO::WaitReadable, Errno::EAGAIN
        break  # Stop reading if no more data is available
      rescue EOFError
        break  # Stop if the connection is closed
      end
    end

    response.join("\n")
  end

end
