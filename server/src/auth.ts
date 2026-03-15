import jwt from "jsonwebtoken";

export type JwtUser = {
  id: number;
  username: string;
  role: "customer" | "admin" | string;
};

export function signToken(user: JwtUser) {
  const secret = process.env.JWT_SECRET;
  if (!secret) throw new Error("JWT_SECRET is not set");
  return jwt.sign(user, secret, { expiresIn: "7d" });
}

export function verifyToken(token: string): JwtUser {
  const secret = process.env.JWT_SECRET;
  if (!secret) throw new Error("JWT_SECRET is not set");
  return jwt.verify(token, secret) as JwtUser;
}
