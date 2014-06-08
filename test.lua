#!/home/simon/devel2/iolua2/iolua
local a,b,c,d,e,f,g,h,i,j
local string = require'string'
print(string.format("--<%d>--",100))
STDIN:read(1000)

STDOUT:write("hallo\n")
STDOUT:write("hallo\n")
STDOUT:write("hallo\n")
STDOUT:write(b)

--[[
function handle_client(client)
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
if srv then srv = nw_async(srv) end
repeat
	local client = srv:accept()
	if client then
		handle_client(client)
	else
		yield()
	end
until nil
-- ]]

--spawn(function() repeat yield() until false end)

--[[
print("Hallo Welt!")
print(listen_ip4)
local listener = listen_ip4("127.0.0.1",1000)
--local listener = listen_ip4("",1000)
print(listener)
local socket = listener:accept()
--print(socket)
socket:write("your name>")
local name = socket:read(100)
STDOUT:write("\nHello "..tostring(name).."\n")
socket:write("\nHello "..tostring(name).."\n")
socket:close()
-- ]]
--[[
spawn(function()
	local i = 0
	yield()
	while i<200 do
		i = i + 1
		print("BBBBB!",i)
	end
end)
local j = 0
while j<200 do
	j = j + 1
	print("AAAAA!",j)
end
-- ]]
