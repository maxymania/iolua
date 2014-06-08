
local __getthread,__runthread,__yield
local __schedule,__requeue,__suspend,__wakeup,__getsuspended,yield
__getthread,__runthread,__yield = ...


local function newParker()
	local Parker = {}
	Parker.i = 0
	Parker.l = 0
	Parker.t = {}
	function Parker.park(obj)
		local i = 0
		while 1 do
			if Parker.i < Parker.l then
				i = Parker.i
				Parker.i = Parker.i + 1
				if not Parker.t[i] then
					Parker.t[i] = obj
					return i
				end
			else
				i = Parker.i
				Parker.i = Parker.i + 1
				Parker.l = Parker.l + 1
				Parker.t[i] = obj
				return i
			end
		end
	end
	function Parker.wake(i)
		if Parker.i>i then Parker.i=i end
		local obj = Parker.t[i]
		Parker.t[i] = nil
		return obj
	end
	return Parker
end

local function newQueue()
	local Queue = {}
	Queue.active = {}
	Queue.passive = {}
	Queue.psize = 0
	Queue.asize = 0
	Queue.apos  = 0
	function Queue.offer(obj)
		Queue.passive[Queue.psize] = obj
		Queue.psize = Queue.psize + 1
	end
	function Queue.flip()
		if not ( Queue.apos < Queue.asize ) then
			Queue.apos = 0
			Queue.asize = Queue.psize
			Queue.psize = 0
			Queue.active, Queue.passive = Queue.passive, Queue.active
		end
	end
	function Queue.take()
		Queue.flip()
		if Queue.asize==0 then return nil end
		local obj = Queue.active[Queue.apos]
		Queue.active[Queue.apos] = nil
		Queue.apos = Queue.apos + 1
		return obj
	end
	return Queue
end

local Scheduler = {}
Scheduler.parked = newParker()
Scheduler.queue  = newQueue()

function __schedule()
	local thr = Scheduler.queue.take()
	if thr then
		__runthread(thr)
	end
	return thr
end

function __requeue()
	local thr = __getthread()
	Scheduler.queue.offer(thr)
end

function __suspend()
	local thr = __getthread()
	return Scheduler.parked.park(thr)
end

function __wakeup(i)
	local thr = Scheduler.parked.wake(i)
	if not (thr==nil) then
		Scheduler.queue.offer(thr)
	end
end

function __getsuspended(i)
	local thr = Scheduler.parked.wake(i)
	return thr
end

function yield()
	__requeue();
	__yield()
end


_G.__requeue=__requeue
_G.__suspend=__suspend
_G.__wakeup=__wakeup
_G.__yield=__yield
_G.yield=yield
return __schedule,__requeue,__suspend,__wakeup,__getsuspended,yield
