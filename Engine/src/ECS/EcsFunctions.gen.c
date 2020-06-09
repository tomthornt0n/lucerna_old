void lc_AddName(lc_Scene* scene, lc_Entity entity, char* name)
{
	scene->EntitySignatures[entity] |= LC__NAME;
	scene->LcName[entity].Name = name;
}

