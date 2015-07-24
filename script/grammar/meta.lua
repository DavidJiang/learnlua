Set = {}
local mt = {}   -- metatable for sets
local prototype = {}

function Set.union (a,b)
    local res = Set.new{}

    for k in pairs(a) do 
        res[k] = true 
    end
    for k in pairs(b) do 
        res[k] = true 
    end
    return res
end
 
function Set.intersection (a,b)
    local res = Set.new{}
    for k in pairs(a) do
       res[k] = b[k]
    end
    return res
end

function Set.tostring (set)
    local s = "{"
    local sep = ""
    for e in pairs(set) do
       s = s .. sep .. e
       sep = ", "
    end
    return s .. "}"
end

function Set.new (t)     -- 2nd version
    local set = {}
    for _, l in ipairs(t) do set[l] = true end
    setmetatable(set, mt)
    return set
end

prototype.test_index_method = function ()
    print("test_index_method is element of mt table.")
end

mt.__add = Set.union
mt.__mul = Set.intersection
mt.__tostring = Set.tostring
mt.__metatable = "mt is metatable of Set." 
mt.__index = function (t,k)
    print("*access to element " .. tostring(k))
    if rawget(t,k) == nil then 
        return prototype[k]  -- access the original table
    else
        return rawget(t,k)
    end
end
mt.__newindex = function (table, key, value)
    print("*update of element " .. tostring(key) .. " to " .. tostring(value))
    if rawget(table,key) == nil then
        if prototype[key] == nil then 
            rawset(table,key,value) --不使用任何元方法,只考虑本表
        else
            prototype[key] = value
        end 
    else
        rawset(table,key,value)
    end
end

s1 = Set.new{1,2}
print("s1: " .. Set.tostring(s1))

s2 = Set.new{2,3}
print("s2: " .. Set.tostring(s2))

s3 = s1+s2

print("s3: " .. Set.tostring(s3))
print("prototype: " .. Set.tostring(prototype))

--print(getmetatable(s1))     --> not your business
--setmetatable(s1, {})

s1.test_index_method()
