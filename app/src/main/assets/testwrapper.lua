---
--- Generated by EmmyLua(https://github.com/EmmyLua)
--- Created by XiongFangyu.
--- DateTime: 2019/3/19 5:08 PM
---

local size = Size()
--- set (Java)Size.width = 1
size:w(1)
--- get (Java)Size.width, set (Java)Size.width to (Java)Size.height
size:h(size:w())
--- call (Java)Size.area() return number value
local area = size:area()
--- call (Java)Size.setCallback
size:setCallback(function ()
    --- called by java when triggerCallback is invoke
    print('callback by java')
end)
--- call (Java)Size.triggerCallback
size:triggerCallback()

--- call (Java)AreaUtils.newSize return userdata value
local otherSize = AreaUtils:newSize(1,1)
--- will call (Java)Size.equals
print('is eqauls:', size == otherSize)
--- call (Java)AreaUtils.csa
print('area by size', area, 'area by AreaUtils', AreaUtils:calSizeArea(otherSize))