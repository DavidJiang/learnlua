
--[[
{first = 0, last = -1}
first,last是数组的两个属性字段,不属于数组的元素
]]
List = {}
function List.new ()
    return {first = 0, last = -1}
end

function List.pushleft (list, value)
    local first = list.first - 1
    list.first = first
    list[first] = value
end
 
function List.pushright (list, value)
    local last = list.last + 1
    list.last = last
    list[last] = value
end
 
function List.popleft (list)
    local first = list.first
    if first > list.last then error("list is empty") end
    local value = list[first]
    list[first] = nil    -- to allow garbage collection
    list.first = first + 1
    return value
end
 
function List.popright (list)
    local last = list.last
    if list.first > last then error("list is empty") end
    local value = list[last]
    list[last] = nil     -- to allow garbage collection
    list.last = last - 1
    return value
end

list_var = List.new();
List.pushright(list_var,2);
List.pushright(list_var,5);
List.pushright(list_var,3);
List.pushright(list_var,100);
List.pushright(list_var,77);
List.pushright(list_var,88);

for k,v in pairs(list_var) do
	print( k .. "   " .. v )
end