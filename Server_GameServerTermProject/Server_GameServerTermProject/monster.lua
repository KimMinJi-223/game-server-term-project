myid = 99999
spawnPos = {x = 0, y = 0}
monsterType = -1
moveState = 0; -- 0 : ����, 1 : ���� ���� ����, 2 : A*(���ݴ��� ����)
roamingArea = 0 -- ���ƴٴϴ� ����
radarArea = 0; -- ���� ��ġ���� ������ ���������Ѱ�?
followArea = 0; -- ������ ������ ������������ ������ ���󰥰ų�?

-- id�� type�� �ָ� �˾Ƽ� �� ���߰� �Ѵ�. 
function set_init(id, x, y, type)
   myid = id;
   monsterType = type;
   spawnPos.x = x;
   spawnPos.y = y;
   if type == 2 then
      moveState = 0;
      roamingArea = 0;
      radarArea = 0;
      followArea = 30;
   elseif type == 3 then
      moveState = 1;
      roamingArea = 20;
      radarArea = 0;
      followArea = 30;
   elseif type == 4 then
      moveState = 0;
      roamingArea = 0;
      radarArea = 5;
      followArea = 30;
   elseif type == 5 then
      moveState = 1;
      roamingArea = 20;
      radarArea = 5;
      followArea = 30;
   end
   --print(moveState, roamingArea, radarArea, followArea);
end

-- Ÿ�Ը��ְ� ���� �Լ����� Ÿ�Կ� ���� �Լ��� ȣ���ϰ� ����
-- 20 20 �������� �����̰� 11�����ȿ� ������ ���� ��ġ���� 30�������� ���󰡱� 
-- ������ �ִ� ���͵� 30�������� ���󰡱�
function IsAStar(playerId, x, y)
    my_x = API_GetPosX(myid);
    my_y = API_GetPosY(myid);
    if ((my_x - x) * (my_x - x) > radarArea * radarArea) then
    API_OkAStar(myid, -1);
    return
	end
	if ((my_y - y) * (my_y - y) > radarArea * radarArea) then
    API_OkAStar(myid, -1);
    return
    end
    API_OkAStar(myid, playerId);
end
