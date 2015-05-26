print('hello world\n')
print('The area is: ' .. args['area'])

if args['area'] == 'water' then
    playSound('Sound/Spit_Splat.wav')
elseif args['area'] == 'fire' then
    playSound('Sound/Large_Fireball.wav')
elseif args['area'] == 'player' then
    playSound('Sound/pissed_off_duck.wav')
elseif args['area'] == 'jump' then
    playSound('Sound/Jump.wav')
elseif args['area'] == 'refill' then
    playSound('Sound/Inflating_Balloon.wav')
elseif args['area'] == 'water_jump' then
    playSound('Sound/Large_Bubble.wav')
elseif args['area'] == 'air' then
    playSound('Sound/Chamber_Decompressing.wav')
elseif args['area'] == 'level_done' then
    playSound('Sound/shooting_star.wav')
end

