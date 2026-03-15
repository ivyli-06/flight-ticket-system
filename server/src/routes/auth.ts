import { Router } from "express";
import { z } from "zod";
import bcrypt from "bcryptjs";
import { pool } from "../db.js";
import { signToken } from "../auth.js";

export const authRouter = Router();

const loginSchema = z.object({
  username: z.string().min(1),
  password: z.string().min(1)
});

/**
 * IMPORTANT:
 * 这里假设 users 表有 password_hash 字段（bcrypt hash）。
 * 你当前 Qt 代码是明文 password 字段；建议你迁移成 password_hash。
 */
authRouter.post("/login", async (req, res) => {
  const parsed = loginSchema.safeParse(req.body);
  if (!parsed.success) return res.status(400).json({ error: "Invalid payload" });

  const { username, password } = parsed.data;

  const [rows] = await pool.execute<any[]>(
    "SELECT id, username, role, password_hash FROM users WHERE username = ? LIMIT 1",
    [username]
  );

  if (!rows.length) return res.status(401).json({ error: "Invalid username or password" });

  const u = rows[0] as { id: number; username: string; role: string; password_hash: string | null };

  if (!u.password_hash) {
    return res.status(500).json({
      error: "User has no password_hash. Please migrate users.password -> users.password_hash (bcrypt)."
    });
  }

  const ok = await bcrypt.compare(password, u.password_hash);
  if (!ok) return res.status(401).json({ error: "Invalid username or password" });

  const token = signToken({ id: u.id, username: u.username, role: u.role });
  return res.json({
    token,
    user: { id: u.id, username: u.username, role: u.role }
  });
});