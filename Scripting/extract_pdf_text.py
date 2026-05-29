from __future__ import annotations

import argparse
from pathlib import Path


def extract_text(pdf_path: Path) -> str:
    try:
        from pypdf import PdfReader
    except ImportError:
        raise SystemExit("Falta la dependencia 'pypdf'. Instala con: pip install pypdf")

    reader = PdfReader(str(pdf_path))
    pages_text: list[str] = []

    for page in reader.pages:
        text = page.extract_text() or ""
        pages_text.append(text.rstrip())

    return "\n\n".join(pages_text).strip()


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Extrae el texto de un PDF y lo guarda en un archivo .txt."
    )
    parser.add_argument("pdf", type=Path, help="Ruta al archivo PDF de entrada")
    parser.add_argument(
        "-o",
        "--output",
        type=Path,
        help="Ruta del archivo de salida .txt. Por defecto usa el mismo nombre del PDF.",
    )
    args = parser.parse_args()

    pdf_path = args.pdf
    if not pdf_path.exists():
        print(f"No existe el archivo: {pdf_path}")
        return 1
    if pdf_path.suffix.lower() != ".pdf":
        print("La entrada debe ser un archivo .pdf")
        return 1

    output_path = args.output or pdf_path.with_suffix(".txt")
    text = extract_text(pdf_path)

    output_path.write_text(text, encoding="utf-8")
    print(f"Texto extraido en: {output_path}")

    if not text:
        print(
            "Aviso: no se extrajo texto. Si el PDF es escaneado, probablemente necesites OCR."
        )

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
