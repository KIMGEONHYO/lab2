use std::io::{self, Write};

fn read_positive_usize(prompt: &str) -> usize {
    loop {
        print!("{prompt}");
        io::stdout().flush().expect("출력 실패");

        let mut input = String::new();
        io::stdin().read_line(&mut input).expect("입력 실패");

        match input.trim().parse::<usize>() {
            Ok(value) if value > 0 => return value,
            _ => println!("0보다 큰 정수를 입력하세요."),
        }
    }
}

fn read_i32(prompt: &str) -> i32 {
    loop {
        print!("{prompt}");
        io::stdout().flush().expect("출력 실패");

        let mut input = String::new();
        io::stdin().read_line(&mut input).expect("입력 실패");

        match input.trim().parse::<i32>() {
            Ok(value) => return value,
            _ => println!("정수를 입력하세요."),
        }
    }
}

fn read_matrix(name: &str, rows: usize, cols: usize) -> Vec<i32> {
    let count = rows.checked_mul(cols).expect("행렬 크기가 너무 큽니다.");
    let mut matrix = Vec::with_capacity(count);

    println!("{name} 행렬의 원소를 입력하세요.");
    for row in 0..rows {
        for col in 0..cols {
            let prompt = format!("{name}[{}, {}] = ", row + 1, col + 1);
            matrix.push(read_i32(&prompt));
        }
    }

    matrix
}

fn print_matrix(matrix: &[i32], rows: usize, cols: usize) {
    for row in 0..rows {
        for col in 0..cols {
            print!("{} ", matrix[row * cols + col]);
        }
        println!();
    }
}

fn main() {
    let rows = read_positive_usize("행의 개수: ");
    let cols = read_positive_usize("열의 개수: ");

    let matrix_a = read_matrix("A", rows, cols);
    let matrix_b = read_matrix("B", rows, cols);

    let sum: Vec<i32> = matrix_a
        .iter()
        .zip(matrix_b.iter())
        .map(|(a, b)| a + b)
        .collect();

    println!("A + B 결과:");
    print_matrix(&sum, rows, cols);
}
