const baseUrl = "https://api.discogs.com/";
const artistId = process.argv[2];

if (!artistId) throw new Error("Please provide an artist ID!");

async function get(path) {
  while (true) {
    const response = await fetch(baseUrl + path);

    if (response.status === 200) {
      try {
        return await response.json();
      } catch (e) {
        throw Error(e, "JSON parsing failed");
      }
    } else if (response.status === 429) {
      process.stderr.write("Rate limit exceeded, waiting 60 seconds...");
      await new Promise((resolve) => setTimeout(resolve, 60 * 1000));
      continue;
    } else if (response.status === 404) {
      throw new Error(`Artist with ID "${artistId}" not found`);
    } else {
      throw new Error(
        `Request failed with status ${response.status}: ${response.statusText}`
      );
    }
  }
}

async function getArtist(artistId) {
  return await get(`artists/${artistId}`);
}

async function getRelatedGroups(artistId) {
  const { members, name } = await getArtist(artistId);

  const membersDetailed = await Promise.all(
    members.map((member) => getArtist(member.id))
  );

  const map = new Map();

  for (m of membersDetailed) {
    for (g of m.groups) {
      if (map.has(g.id)) {
        map.get(g.id).members.push(m.name);
      } else {
        map.set(g.id, { group: g.name, members: [m.name] });
      }
    }
  }

  return map;
}

getRelatedGroups(artistId)
  .then((map) =>
    [...map.values()]
      .filter(({ members }) => members.length > 1)
      .sort((a, b) => a.group.localeCompare(b.group))
      .map(({ group, members }) => `${group}: ${members.sort().join(", ")}`)
      .forEach((line) => process.stdout.write(line + "\n"))
  )
  .catch((e) => {
    process.stderr.write(e.message + "\n");
    process.exit(1);
  });
