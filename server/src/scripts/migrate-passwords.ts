import "dotenv/config";
import bcrypt from "bcryptjs";
import { pool } from "../db.js";

async function main() {
  // 取所有用户的旧明文密码
  const [rows] = await pool.execute<any[]>(
    "SELECT id, password FROM users WHERE password_hash IS NULL"
  );

  console.log(`Users to migrate: ${rows.length}`);

  for (const r of rows) {
    const id = Number(r.id);
    const plain = String(r.password ?? "");

    if (!plain) {
      console.warn(`Skip id=${id} (empty password)`);
      continue;
    }

    const hash = await bcrypt.hash(plain, 10); // cost=10（够用且不会太慢）
    await pool.execute("UPDATE users SET password_hash = ? WHERE id = ?", [hash, id]);
    console.log(`Migrated id=${id}`);
  }

  await pool.end();
  console.log("Done.");
}

main().catch((e) => {
  console.error(e);
  process.exit(1);
});