--local a = 1
local b = 'eisd,adickdads'
local c = nil

function f1(d)
    local lud = LazyUD()
    return lud:a('1', 1)
end
function f2(d)
    print('f2')
    return f1(d)
end
function f3(d)
    print('f3')
    return f2(d);
end

--for i = 1, 100 do
--    --a = a + i;
--    c = string.find(b, 'ickd')
    print("StaticClassTest a result", StaticClassTest:a(1))
--end
--print('after 100 loop c = ', c)
local start = os.clock();
local testud = UDT(1,'a')
--print('do error code')
print('doing: ', testud:a(f3):a())
--print('after')
local endTime = os.clock();
