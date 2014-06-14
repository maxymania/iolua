iolua
=====

Lua version with Boostet Network IO using libev and Threaded File IO

Example
=======
```lua
function handle_client(client)
	-- we're going to run the handler-code in a lightweight thread
	-- these implement preemptive multithreading in usermode level.
	spawn(function()
		repeat
			local result = client:read(1024)
			print("result",result)
			if type(result)=='string' then
				client:write(result)
			else
				break
			end
		until nil
		print("EOF!")
		client:close()
	end)
end

local srv = listen_in4("127.0.0.1",1000)
-- the server socket 'srv' is blocking/unboosted, that's why we create a
-- asynchronous/nonblocking version of it.
if srv then srv = nw_async(srv) end

repeat
	-- each call to a nonblocking libev-boosted network function acts like a
	-- blocking function by suspending the current lua coroutine
	local client = srv:accept()
	if client then
		handle_client(client)
	else
		yield()
	end
until nil
```
Build
=====

To build do the following steps

- Make the libev in the libev Folder (but dont do `make install`) (You need to `./configure` before make.)
- Go back to iolua folder
- (OPTIONAL) edit ./build.sh and edit LIBEV_A
- run ./build.sh

