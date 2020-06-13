void lc_AddName(lcScene_t* scene, lcEntity_t entity, char* name)
{
	scene->EntitySignatures[entity] |= LC__NAME;
	scene->Name[entity].Name = name;
}

