local player = nil
local bulletCount = 0
local fromLeftGun = true
local timeSinceLastShot = 11

function READY()
	globals.bullets = globals.bullets or {}

	player = scene:GetGameObject("Player")
	player.shots = 0
end

function UPDATE()
	if input.IsKeyDown("q") then
		timeSinceLastShot = timeSinceLastShot + 1
		if timeSinceLastShot > 0 then
			timeSinceLastShot = 0

			player.shots = player.shots + 1

			local obj = GameObject.new()
			bulletCount = bulletCount + 1

			fromLeftGun = not fromLeftGun -- Switch which gun we're shooting from

			local left = vec2.new(math.sin(math.rad(player:GetRotation() - 90)), math.cos(math.rad(player:GetRotation() - 90)))
			local up = vec2.new(math.sin(math.rad(player:GetRotation())), math.cos(math.rad(player:GetRotation())))

			if fromLeftGun then
				obj:SetPosition(player:GetPosition() + left * 1.1 + up * 1.2)
			else
				obj:SetPosition(player:GetPosition() - left * 1.1 + up * 1.2)
			end

			obj:SetScale(vec2.new(0.1, 0.2))
			obj:SetRotation(player:GetRotation())
			obj:SetColor(vec3.new(1, 0, 0))

			local bulletName = "bullet" .. bulletCount
			scene:AddGameObject(bulletName, obj)

			-- Store both the name and the object in the bullets list
			table.insert(globals.bullets, {name = bulletName, object = obj})

			player:Translate(up * -0.025)
		end
	end

	-- Loop through the bullets list and move each bullet
	for i = #globals.bullets, 1, -1 do
		local bulletData = globals.bullets[i]
		local bullet = bulletData.object  -- Access the bullet object directly
		local radians = math.rad(bullet:GetRotation())
		local direction = vec2.new(math.sin(radians), math.cos(radians))
		bullet:Translate(direction * 0.5)

		-- Check if the bullet is too far from the player and remove it if necessary
		if bullet:GetPosition():Distance(player:GetPosition()) > 250 then
			scene:RemoveGameObject(bulletData.name)
			table.remove(globals.bullets, i)
		end
	end
end
