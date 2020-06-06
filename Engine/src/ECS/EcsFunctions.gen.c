void lc_AddTransform(lc_Scene* scene, lc_Entity entity, float xpos, float ypos, float xscale, float yscale, float rotation)
{
	lc_AddComponent(scene, entity, LC__TRANSFORM);
	scene->LcTransform[entity].XPos = xpos;
	scene->LcTransform[entity].YPos = ypos;
	scene->LcTransform[entity].XScale = xscale;
	scene->LcTransform[entity].YScale = yscale;
	scene->LcTransform[entity].Rotation = rotation;
}

void lc_AddColour(lc_Scene* scene, lc_Entity entity, float red, float green, float blue, float alpha)
{
	lc_AddComponent(scene, entity, LC__COLOUR);
	scene->LcColour[entity].Red = red;
	scene->LcColour[entity].Green = green;
	scene->LcColour[entity].Blue = blue;
	scene->LcColour[entity].Alpha = alpha;
}

void lc_AddTexture(lc_Scene* scene, lc_Entity entity, uint32_t texture_id)
{
	lc_AddComponent(scene, entity, LC__TEXTURE);
	scene->LcTexture[entity].TextureID = texture_id;
}

void lc_AddSubSprite(lc_Scene* scene, lc_Entity entity, float xsize, float ysize, float xoffset, float yoffset)
{
	lc_AddComponent(scene, entity, LC__SUB_SPRITE);
	scene->LcSubSprite[entity].XSize = xsize;
	scene->LcSubSprite[entity].YSize = ysize;
	scene->LcSubSprite[entity].XOffset = xoffset;
	scene->LcSubSprite[entity].YOffset = yoffset;
}

void lc_AddName(lc_Scene* scene, lc_Entity entity, char* name)
{
	lc_AddComponent(scene, entity, LC__NAME);
	scene->LcName[entity].Name = name;
}

void lc_AddHealth(lc_Scene* scene, lc_Entity entity, int health, int max_health)
{
	lc_AddComponent(scene, entity, HEALTH);
	scene->Health[entity].Health = health;
	scene->Health[entity].MaxHealth = max_health;
}

