import { useState } from "react";

const cards = [
  {
    id: 0,
    section: "Descripción del Prototipo",
    time: "~1 min 30 seg",
    color: "#ff8800",
    accent: "#ffaa44",
    bg: "#1a0d00",
    points: [
      "Proyecto: Trinidad E-Commerce Pipeline — simula pedidos en tiempo real",
      "Imaginen cientos de pedidos llegando desde México, Colombia, Francia, USA al mismo tiempo",
      "El sistema recibe, procesa, detecta fraudes y guarda datos válidos — en milisegundos",
      "Stack: Python · Kafka · Ignite · Kudu · PySpark · Grafana",
      "Python genera los pedidos simulados (ventas, productos, zonas)",
      "Clasifica cada pedido por zona geográfica y por precio",
      "País riesgoso o demasiadas compras en poco tiempo → bloqueado automáticamente",
    ],
    tip: "Hablar despacio al principio — es la introducción al prototipo para todo el público.",
    script: "Gracias. Ahora les voy a hablar del prototipo que construimos para poner en práctica todo lo que mis compañeros explicaron.",
  },
  {
    id: 1,
    section: "Kafka en el Prototipo",
    time: "~40 seg",
    color: "#00c896",
    accent: "#00ffb3",
    bg: "#001a10",
    points: [
      "FastAPI es la puerta de entrada — recibe pedidos por API REST",
      "Publica cada pedido en Kafka usando country_code como clave de particionamiento",
      "Partición 0: pedidos de US y CA",
      "Partición 1: pedidos de ES y FR",
      "Partición 2: resto del mundo",
      "Garantiza orden y localidad por país",
      "Si un consumidor falla → el mensaje no se pierde, sigue en Kafka esperando",
      "Composibilidad: agregar un nuevo consumidor sin tocar nada más",
    ],
    tip: "Mencionar que Kafka es el 'sistema nervioso' que conecta todo. No entrar en detalles técnicos del broker.",
    script: "El flujo empieza con FastAPI. Recibe los pedidos y los publica en Kafka usando el country_code como clave.",
  },
  {
    id: 2,
    section: "Ignite en el Prototipo",
    time: "~40 seg",
    color: "#e04040",
    accent: "#ff6b6b",
    bg: "#1a0a0a",
    points: [
      "Toma los pedidos de Kafka en tiempo real",
      "Caché 1 — risk_countries: lista estática de países bloqueados",
      "Caché 2 — user_velocity: contador de compras por usuario en el último minuto",
      "user_velocity detecta bots y comportamientos anormales",
      "Resultado por pedido: APPROVED ✓ o BLOCKED ✗",
      "Todo en RAM — ultrarrápido, sin tocar disco",
      "Los pedidos APPROVED pasan al siguiente topic: orders-processed",
    ],
    tip: "Pausar al mencionar los dos cachés — es el corazón de la detección de fraude. Puede caer pregunta aquí.",
    script: "Una vez en Kafka, Ignite toma cada pedido. En memoria mantiene dos cachés...",
  },
  {
    id: 3,
    section: "Kudu en el Prototipo",
    time: "~40 seg",
    color: "#00aadd",
    accent: "#44ccff",
    bg: "#00101a",
    points: [
      "Solo recibe los pedidos APPROVED del topic orders-processed",
      "PySpark actúa como puente entre Python y el clúster de Kudu (vía JVM + Py4J)",
      "Almacena en formato columnar → ideal para consultas analíticas",
      "Usa upsert en vez de insert → garantiza idempotencia",
      "Idempotencia: si Kafka reintenta un mensaje, no se duplica en Kudu",
      "Datos repartidos en Tablets por país (US-CA / ES-FR / Resto)",
      "Grafana conecta directamente aquí para el dashboard",
    ],
    tip: "Explicar upsert con una frase simple: 'si el pedido ya existe, lo actualiza; si no, lo inserta'. No asumir que todos conocen el término.",
    script: "Los pedidos aprobados pasan a Kudu usando PySpark como puente...",
  },
  {
    id: 4,
    section: "Muestra del Prototipo",
    time: "~1 min 30 seg",
    color: "#ff8800",
    accent: "#ffaa44",
    bg: "#1a0d00",
    points: [
      "[ Abrir Grafana en http://localhost:3000 ]",
      "KPIs superiores: Ingresos Totales · Total Órdenes · Ticket Promedio",
      "Gráfico de barras: ingresos por país (México, USA, Honduras, Colombia, Argentina)",
      "Gráfico de dona: Aprobadas vs Bloqueadas por fraude",
      "[ Si el sistema corre en vivo ] señalar cómo los números cambian solos",
      "Grafana refresca cada 5 segundos — los datos son en tiempo real",
      "Cierre: cada tecnología hace lo que mejor sabe; juntas forman un sistema real",
    ],
    tip: "PLAN B si el demo falla: usar las capturas de pantalla del PDF. Tenerlas abiertas en otra pestaña antes de empezar.",
    script: "Pasemos a verlo funcionando. [abrir Grafana] Acá pueden ver los datos en tiempo real...",
  },
];

export default function CueCardsAbae() {
  const [current, setCurrent] = useState(0);
  const [showScript, setShowScript] = useState(false);
  const card = cards[current];

  return (
    <div style={{
      minHeight: "100vh",
      background: "#0d0d0d",
      display: "flex",
      flexDirection: "column",
      alignItems: "center",
      justifyContent: "center",
      fontFamily: "'Courier New', monospace",
      padding: "2rem 1rem",
      boxSizing: "border-box",
    }}>
      <h2 className="sr-only">Cue cards de Abraham — Prototipo E-Commerce BD2</h2>

      <div style={{
        width: "100%",
        maxWidth: 700,
        display: "flex",
        justifyContent: "space-between",
        alignItems: "center",
        marginBottom: "1.2rem",
      }}>
        <div style={{ display: "flex", alignItems: "center", gap: 10 }}>
          <span style={{
            fontSize: 12,
            fontWeight: 700,
            color: "#ff8800",
            border: "1px solid #ff8800",
            padding: "3px 10px",
            borderRadius: 2,
            letterSpacing: 1,
            background: "#ff880018",
          }}>@Abæ</span>
          <span style={{ color: "#444", fontSize: 12, letterSpacing: 2 }}>PROTOTIPO</span>
        </div>
        <span style={{ color: "#555", fontSize: 13 }}>{current + 1} / {cards.length}</span>
      </div>

      <div style={{
        width: "100%",
        maxWidth: 700,
        background: card.bg,
        border: `2px solid ${card.color}`,
        borderRadius: 4,
        padding: "2rem 2.5rem",
        boxSizing: "border-box",
        position: "relative",
        transition: "border-color 0.2s ease",
      }}>
        <div style={{
          position: "absolute",
          top: 0, left: 0, right: 0,
          height: 3,
          background: card.color,
          borderRadius: "2px 2px 0 0",
        }} />

        <div style={{
          display: "flex",
          justifyContent: "space-between",
          alignItems: "flex-start",
          marginBottom: "1.5rem",
          flexWrap: "wrap",
          gap: 8,
        }}>
          <div>
            <h3 style={{
              margin: "0 0 4px",
              fontSize: 20,
              fontWeight: 700,
              color: card.accent,
              letterSpacing: 1,
              textTransform: "uppercase",
            }}>
              {card.section}
            </h3>
            <span style={{
              fontSize: 11,
              color: card.color,
              letterSpacing: 1,
              opacity: 0.8,
            }}>
              ⏱ {card.time}
            </span>
          </div>
          <button
            onClick={() => setShowScript(s => !s)}
            style={{
              background: showScript ? card.color + "22" : "transparent",
              border: `1px solid ${card.color}`,
              color: card.accent,
              padding: "4px 12px",
              fontFamily: "'Courier New', monospace",
              fontSize: 11,
              letterSpacing: 1,
              cursor: "pointer",
              textTransform: "uppercase",
              borderRadius: 2,
              transition: "all 0.15s",
            }}
          >
            {showScript ? "▼ SCRIPT" : "▶ SCRIPT"}
          </button>
        </div>

        {showScript && (
          <div style={{
            background: card.color + "12",
            border: `1px solid ${card.color}44`,
            borderLeft: `3px solid ${card.color}`,
            padding: "10px 14px",
            borderRadius: 2,
            marginBottom: "1.2rem",
          }}>
            <span style={{ fontSize: 10, color: card.color, fontWeight: 700, letterSpacing: 2, textTransform: "uppercase" }}>
              🎤 FRASE DE ENTRADA
            </span>
            <p style={{ margin: "6px 0 0", fontSize: 13, color: "#ccc", lineHeight: 1.6, fontStyle: "italic" }}>
              "{card.script}"
            </p>
          </div>
        )}

        <ul style={{ margin: "0 0 1.5rem", padding: 0, listStyle: "none" }}>
          {card.points.map((p, i) => (
            <li key={i} style={{
              display: "flex",
              alignItems: "flex-start",
              gap: 10,
              padding: "7px 0",
              borderBottom: `1px solid ${card.color}22`,
              color: p.startsWith("[") ? card.accent : "#d0d0d0",
              fontSize: 14,
              lineHeight: 1.6,
              fontStyle: p.startsWith("[") ? "italic" : "normal",
            }}>
              <span style={{
                color: card.accent,
                fontWeight: 700,
                minWidth: 20,
                marginTop: 1,
                flexShrink: 0,
                fontSize: p.startsWith("[") ? 12 : 14,
              }}>
                {p.startsWith("[") ? "▶" : String(i + 1).padStart(2, "0")}
              </span>
              <span>{p}</span>
            </li>
          ))}
        </ul>

        <div style={{
          background: card.color + "18",
          border: `1px solid ${card.color}55`,
          borderLeft: `3px solid ${card.color}`,
          padding: "10px 14px",
          borderRadius: 2,
        }}>
          <span style={{ fontSize: 11, color: card.color, fontWeight: 700, letterSpacing: 2, textTransform: "uppercase" }}>
            💡 TIP
          </span>
          <p style={{ margin: "4px 0 0", fontSize: 13, color: "#aaa", lineHeight: 1.5 }}>
            {card.tip}
          </p>
        </div>
      </div>

      <div style={{
        width: "100%",
        maxWidth: 700,
        display: "flex",
        justifyContent: "space-between",
        alignItems: "center",
        marginTop: "1.5rem",
        gap: 12,
      }}>
        <button
          onClick={() => { setCurrent(c => Math.max(0, c - 1)); setShowScript(false); }}
          disabled={current === 0}
          style={{
            background: "transparent",
            border: `1px solid ${current === 0 ? "#333" : "#666"}`,
            color: current === 0 ? "#333" : "#aaa",
            padding: "10px 24px",
            fontFamily: "'Courier New', monospace",
            fontSize: 13,
            letterSpacing: 2,
            cursor: current === 0 ? "not-allowed" : "pointer",
            textTransform: "uppercase",
            borderRadius: 2,
            transition: "all 0.15s",
          }}
        >
          ← PREV
        </button>

        <div style={{ display: "flex", gap: 6 }}>
          {cards.map((c, i) => (
            <button
              key={i}
              onClick={() => { setCurrent(i); setShowScript(false); }}
              style={{
                width: i === current ? 24 : 8,
                height: 8,
                borderRadius: 4,
                background: i === current ? cards[i].color : "#333",
                border: "none",
                cursor: "pointer",
                padding: 0,
                transition: "all 0.2s ease",
              }}
              aria-label={`Ir a tarjeta ${i + 1}`}
            />
          ))}
        </div>

        <button
          onClick={() => { setCurrent(c => Math.min(cards.length - 1, c + 1)); setShowScript(false); }}
          disabled={current === cards.length - 1}
          style={{
            background: "transparent",
            border: `1px solid ${current === cards.length - 1 ? "#333" : card.color}`,
            color: current === cards.length - 1 ? "#333" : card.accent,
            padding: "10px 24px",
            fontFamily: "'Courier New', monospace",
            fontSize: 13,
            letterSpacing: 2,
            cursor: current === cards.length - 1 ? "not-allowed" : "pointer",
            textTransform: "uppercase",
            borderRadius: 2,
            transition: "all 0.15s",
          }}
        >
          NEXT →
        </button>
      </div>

      <div style={{
        marginTop: "1.5rem",
        display: "flex",
        gap: 8,
        flexWrap: "wrap",
        justifyContent: "center",
        maxWidth: 700,
      }}>
        {cards.map((c, i) => (
          <button
            key={i}
            onClick={() => { setCurrent(i); setShowScript(false); }}
            style={{
              background: i === current ? c.color + "22" : "transparent",
              border: `1px solid ${i === current ? c.color : "#333"}`,
              color: i === current ? c.accent : "#555",
              padding: "5px 12px",
              fontFamily: "'Courier New', monospace",
              fontSize: 11,
              letterSpacing: 1,
              cursor: "pointer",
              textTransform: "uppercase",
              borderRadius: 2,
              transition: "all 0.15s",
            }}
          >
            {c.section}
          </button>
        ))}
      </div>

      <div style={{
        marginTop: "1.2rem",
        color: "#333",
        fontSize: 11,
        letterSpacing: 1,
        textAlign: "center",
      }}>
        TOTAL ~5 MIN · 5 TARJETAS
      </div>
    </div>
  );
}
